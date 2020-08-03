// aml_encrypt_g12b --bl3sig
//
// This is supposed to replicate this
// ./aml_encrypt_g12b --bl3sig  --input bl30_new.bin.g12a.enc --output bl30_new.bin.enc      --level v3 --type bl30
// ./aml_encrypt_g12b --bl3sig  --input bl31.img              --output bl31.img.enc          --level v3 --type bl31
// ./aml_encrypt_g12b --bl3sig  --input bl33.bin              --output bl33.bin.enc          --level v3 --type bl33 --compress lz4

#include "types.h"
#include "lib.h"
#include "lz4hc.h"

int main(int argc, char ** argv) {

    opt_values_bl3sig_t opt_values;
    char **opt_array_view = (char**)&opt_values;

    option_t opts[21] = {
        { "input",      1, 0, 0x30 },       // 0x00
        { "amluserkey", 1, 0, 0x31 },       // 0x01
        { "userkey",    1, 0, 0x32 },       // 0x02
        { "output",     1, 0, 0x33 },       // 0x03
        { "skipsector", 1, 0, 0x34 },       // 0x04
        { "level",      1, 0, 0x35 },       // 0x05
        { "type",       1, 0, 0x36 },       // 0x06
        { "compress",   1, 0, 0x37 }        // 0x07
    };

    bzero(&opt_values, sizeof opt_values);
    int getopt_result = 0;
    while(getopt_long(argc, argv, "", opts, &getopt_result) != -1)
        opt_array_view[getopt_result] = optarg;

    struct stat stat_info;

    if(opt_values.input == NULL || stat(opt_values.input, &stat_info) != 0)
        exit(1);

    char trusted_payload_fname[0x100];
    char output_file_name[0x100];
    bzero(output_file_name, 0x100);

    /* try to fixup the output filename if it's not provied */
    if(opt_values.output == NULL) {
        if(0 == stat(opt_values.amluserkey, &stat_info) ||
           0 == stat(opt_values.userkey, &stat_info)) {
            sprintf(output_file_name, "%s%s", opt_values.input, ".sig");
        } else {
            sprintf(output_file_name, "%s%s", opt_values.input, ".pkg");
        }
        opt_values.output = output_file_name;
    }

    /* onto the signing */
    char *key_file = opt_values.userkey != NULL
                   ? opt_values.userkey
                   : opt_values.amluserkey;

    bool_t withUserKey = 0 == stat(key_file, &stat_info);

    bool_t rsa_key = false;
    // rsa_init(&rsa_ctx, 0, 0);
    if(withUserKey) {
        // rsa_key = load_rsa_key_file(rsa_ctx, keyfile);
    }
    if(!rsa_key) {
        bzero(trusted_payload_fname, 0x100);

        sprintf(trusted_payload_fname, "%s.%09d.tp", opt_values.output, rand());

        off_t header_offset = 0;
        off_t payload_offset = 0x490;

        uint8_t in_header[0x50];
        uint8_t out_header[0x100];

        bzero(out_header, 0x100);

        *(uint64_t*)out_header = 0x5244482d58334c42;
        *(uint32_t*)&out_header[0x08] = 0x100;
        if(0 == stat(opt_values.input, &stat_info)) {
            FILE *fin = fopen(opt_values.input, "rb");
            // trusted_payload_fname = $output.$rnd.tp
            FILE *fout = fopen(trusted_payload_fname, "wb");
            if(NULL != fin && NULL != fout) {
                fread(in_header, 1, 0x50, fin);
                if(*(uint32_t*)in_header == 0x12348765) {
                    header_offset = 0x200;
                    payload_offset = 0x590;
                    memcpy(&out_header[0x10], &in_header[0x08], 0x10);
                    *(uint32_t*)&out_header[0x20] = *(uint32_t*)&in_header[0x20];
                    *(uint32_t*)&out_header[0x28] = *(uint32_t*)&in_header[0x18];
                    *(uint32_t*)&out_header[0x2c] = *(uint32_t*)&in_header[0x28];
                    memcpy(&out_header[0x30], &in_header[0x00], 0x50);
                }
                // copy header from input to tp file.
                fseek(fin, header_offset, SEEK_SET);
                file_copy(fin, fout, stat_info.st_size - header_offset);
            }
            if(NULL != fin) fclose(fin);
            if(NULL != fout) fclose(fout);

            if(NULL != opt_values.compress &&
               0 == strncmp(opt_values.compress, "lz4",3)) {

                char lz4_fname[0x100];
                bzero(lz4_fname,0x100);
                sprintf(lz4_fname,"%s.lz4",trusted_payload_fname);

                lz4_header_t lz4_header;
                bzero(&lz4_header, 0x80);

                lz4_header.magic = 0x43345a4c; /* "C4LZ" */
                lz4_header.magic_2 = 0x43345a4c;
                lz4_header.header_size = 0x80;

                // can we live with a zero nonce?
                lz4_header.random_nonce[0] = 0x0;

                time_t timep;
                time(&timep);
                struct tm *local_time = localtime(&timep);

                sprintf(lz4_header.timestamp, "%04d%02d%02d%02d:%02d:%02d",
                        local_time->tm_year + 1900,
                        local_time->tm_mon + 1,
                        local_time->tm_mday,
                        local_time->tm_hour,
                        local_time->tm_min,
                        local_time->tm_sec);

                if(0 == stat(trusted_payload_fname, &stat_info)) {
                    file_sha256(trusted_payload_fname, lz4_header.file_sha256);
                    void *payload = malloc(stat_info.st_size);
                    if(NULL != payload) {
                        FILE *fin = fopen(trusted_payload_fname, "rb");
                        if (NULL == fin) exit(1);
                        fread(payload, 1, stat_info.st_size, fin);
                        fout = fopen(lz4_fname, "wb");
                        if (NULL != fout) {
                            void *dst = malloc(stat_info.st_size+0x400);
                            if(NULL != dst) {
                                lz4_header.original_size = stat_info.st_size;
                                lz4_header.compressed_size
                                    = LZ4_compress_HC(payload, dst,
                                                    lz4_header.original_size,
                                                    stat_info.st_size + 0x400,
                                                    0xc);
                                sha256((uchar_t *)&lz4_header, 0x60, lz4_header.header_sha256);
                                fwrite((uchar_t *)&lz4_header, 1, 0x80, fout);
                                fwrite(dst, 1, lz4_header.compressed_size, fout);
                                free(dst);
                            }
                            fclose(fout);
                        }
                        fclose(fin);
                        free(payload);
                        // copy file from the lz4 ext to the original .tp file.
                        // and unlink the lz4 one.
                        // for now we'll just call the tp file the lz4 file.
                        memcpy(trusted_payload_fname, lz4_fname, 0x100);
                    }
                }
            }
            /* ok so we did the optional lz4 compression */
            if(0 == stat(trusted_payload_fname, &stat_info)) {
                size_t size16k = (stat_info.st_size + 0x48f & 0xfffffe00) - 0x290;
                file_extend(trusted_payload_fname, size16k, 0);

                // The asumption here is that we successfully copied trusted_payload_fname.

                bl3_sig_header_t sig_header;
                bzero(&sig_header, 0x290);
                sig_header.magic = 0x4c4d4140; // @AML
                sig_header.unk1  = 0x1;
                sig_header.size16k = size16k;
                sig_header.size  = 0x290;

                // if (withUserKey) {
                //     // open keyfile
                //     // read the last 32byte from the key file
                //     // into a 0x30 zeroed buffer.
                //     // write them into sig_header.key

                //     // if --type is NULL, abort.
                //     // some logic depending on the --type name
                // }
                file_sha256(trusted_payload_fname, sig_header.file_sha256);

                size_t header_length = 0x260; // this is 0x280 - size of rsa key or something if withUserKey
                sha256((uchar_t*)&sig_header.magic, header_length, sig_header.sha256);

                // do some rsa_pkcs1 signing if withUserKey

                uint8_t header_buf[0x2000];
                bzero(&header_buf, 0x2000);
                *(uint32_t*)&header_buf[0x0] = 0x59454b40; // @KEY
                *(uint32_t*)&header_buf[0x4] = 0x30;
                *(uint16_t*)&header_buf[0x8] = 0x1;
                *(uint32_t*)&header_buf[0xa] = 0x30;
                size_t header_buf_size = 0x490;

                // if (withUserKey) ...

                FILE *fout = fopen(opt_values.output, "wb");
                if(fout != NULL) {
                    /* header_offset == 0, unless --input starts with 0x12348765 */
                    if(header_offset > 0)
                        fwrite(&out_header, 1, 0x100, fout);
                    /* header_buf_size is 0x490 */
                    fwrite(&header_buf, 1, header_buf_size, fout);
                    /* sig_header will be written 0x290 -- 0x720 */
                    fwrite(&sig_header, 1, 0x290, fout);
                    fclose(fout);
                }
                /* output_file_name = output_file_name + trusted_payload_fname */
                file_append(trusted_payload_fname, opt_values.output);
                // if(withUserKey) ...
            }
        }
        // drop the fp_fname
        unlink(trusted_payload_fname);
    } else {
        /* rsa_key is true */
    }
}