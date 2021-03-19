// aml_encrypt_g12b --bl3sig
//
// This is supposed to replicate this
// ./aml_encrypt_g12b --bl30sig --input bl30_new.bin \
//                              --output bl30_new.bin.g12a.enc --level v3

#include <string.h>

#include "types.h"
#include "lib.h"

int main(int argc, char ** argv) {

    opt_values_bl30sig_t opt_values;
    char **opt_array_view = (char**)&opt_values;

    option_t opts[21] = {
        { "input",      1, 0, 0x30 },       // 0x00
        { "amluserkey", 1, 0, 0x31 },       // 0x01
        { "userkey",    1, 0, 0x32 },       // 0x02
        { "keymax",     1, 0, 0x33 },       // 0x03
        { "aeskey",     1, 0, 0x34 },       // 0x04
        { "output",     1, 0, 0x35 },       // 0x05
        { "efuse",      1, 0, 0x36 },       // 0x06
        { "skipsector", 1, 0, 0x37 },       // 0x07
        { "level",      1, 0, 0x38 }        // 0x08
    };

    bzero(&opt_values, sizeof opt_values);
    int getopt_result = 0;
    while(getopt_long(argc, argv, "", opts, &getopt_result) != -1)
        opt_array_view[getopt_result] = optarg;

    struct stat stat_info;

    if(opt_values.input == NULL || stat(opt_values.input, &stat_info) != 0)
        exit(1);

    bool_t withUserKey = !(opt_values.amluserkey == NULL && (opt_values.userkey == NULL || opt_values.keymax == NULL));

    size_t skip = 0;

    if(opt_values.skipsector != NULL) skip = strtouq(opt_values.skipsector, NULL, 0x10);
    if(skip > 4) skip = 4;

    if(withUserKey && skip != 0)
        exit(1);

    char output_filename[0x100];
    bzero(output_filename, 0x100);
    if(opt_values.output == NULL) {
        sprintf(output_filename, "%s%s", opt_values.input, withUserKey ? ".encrypt" : ".pkg");
        opt_values.output = output_filename;
    } else {
        strcpy(output_filename, opt_values.output);
    }

    if(0 == stat(opt_values.input, &stat_info)) {
        size_t input_size16k = stat_info.st_size + 0x1ff & 0xffffe00;
        if (input_size16k > 0xf000) {
            printf("BL30 file length [%s] len=0x%x, max is 0x%x\n",
                   opt_values.input,
                   (uint32_t)input_size16k,
                   0xf000);
            exit(-INPUTBADSIZE);
        }
        FILE *fin = fopen(opt_values.input, "rb");
        unlink(opt_values.output);
        FILE *fout = fopen(opt_values.output, "wb+");

        uint8_t *payload = calloc(1,0x10000);
        if(NULL == payload) exit(-ENOMEM);

        uint8_t nonce[0x10];
        /* inject 16 bytes of random nonce */
        for(int i = 0; i < 0x10; i++)
            nonce[i] = rand();

        /* if(withUserKey) ... */

        bl30_sig_header_t header;
        bzero(&header, 0x40);
        header.magic = 0x4c4d4140; // @AML
        header.total_size = 0x40; // for now.
        header.hdr_size = 0x40;
        header.unk1 = 0x1;
        header.unk2 = 0x1;

        /* if (withUserKey && key_file != NULL) */
        {
            fwrite(nonce, 1, 0x10, fout);
        }

        bzero(payload, 0x10000);
        fread(payload, 1, input_size16k, fin);
        if(0x4c4d4140 == *(uint32_t*)&payload[0x02] &&
           *(uint32_t*)&payload[0x14] == *(uint32_t*)&payload[0x2c] + *(uint32_t *)&payload[0x3c]) {
            bzero(payload, 0x10000);
            fseek(fin, 0x1000, SEEK_SET);
            fread(payload, 1, input_size16k, fin);
        }
        fclose(fin);
        fin = NULL;

        // So the output has the following soncstruction:
        // -0x10   <nonce:0x10>
        // 0x00   <header:0x40>
        // 0x40   <???:0x200>   -- signature
        //   0x40   <sha256(header):0x20>
        // 0x240  <???:0x30> -- key info
        //   0x240  <key.magic("@KEY"):0x4>
        //   0x244  <key.size:0x4>
        //   0x248  <key.unk0:0x1>
        //   0x249  <key.len:0x1>
        //   0x24a  <key.unk1:0x1>
        //   0x24b  <key.0-pad:0x5>
        //   0x250  <key.sha256:0x20>
        // 0x270  <???:0xd80> -- we write in payload2:0xd80 here.
        // 0xff0  <payload>


        size_t payload2_size = 0xd80;
        size_t sha256size = 0x20;
        /* if (withUserKey) ... */
        /* else */
        {
            if(skip != 0)
                sha256size = skip * 0x200 - 0x50;

            header.encrypted = 0;
            header.key_len = 0;
            header.payload2_with_key = 0xdb0;
        }
        header.block_size = 0x200;
        header.header_size = header.hdr_size;
        header.sig_body_offset = sha256size + header.header_size;
        header.key_offset = header.header_size + 0x200;
        header.payload_offset = header.payload2_with_key + header.key_offset;
        header.total_size = input_size16k + header.payload2_with_key + header.total_size + 0x200;
        header.body_size = header.total_size - header.sig_body_offset;
        header.input_size16k = input_size16k;

        uint8_t *buf = (uint8_t*)calloc(1, header.total_size);
        if(buf == NULL) exit(-ENOMEM);

        memcpy(&buf[0x0], &header, header.hdr_size);
        /* if(withUserKey) ... */

        /* 0xff0 = 0x40 + 0x200 + 0x30 + 0xd80 */
        memcpy(&buf[0xff0], payload, input_size16k);

        /* verify our sizes are correct */
        if(&buf[0x0] + header.total_size != &buf[0xff0] + header.input_size16k) {
            printf("calucation failed %04x != %04x\n", header.total_size, 0xff0 + header.input_size16k);
            exit(-EINVAL);
        }

        /* if(withUserKey) ... */
        /* else */
        {
            *(uint32_t*)&buf[0x24] = 0x240;
            *(uint16_t*)&buf[0x258] = 0x298;
            *(uint32_t*)&buf[0x8ec] = 0x240;
            *(uint16_t*)&buf[0xb20] = 0x298;
        }
        /* if(withUserKey) ...
            *   compute the sha256 as below, but pkcs1_sign it
            *   before writing it into &buf[0x40]
            */
        /* else */
        {
            sha256n(2,
                    (uchar_t*[]){&buf[0x0], &buf[0x60]},
                    (size_t[]){header.hdr_size, header.body_size},
                    &buf[0x40]);
        }
        fwrite(&buf[0x0], 1, header.total_size, fout);
        fclose(fout);

        exit(0);
    }
}
