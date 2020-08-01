// aml_encrypt_g12b --bootmk
//
// This command produced the final boot image
// for Amlogic S922X systems.
// ./aml_encrypt_g12b --bootmk \
//                    --output u-boot.bin \
//                    --level v3 \
//                    --bl2  bl2.n.bin.sig \
//                    --bl30 bl30_new.bin.enc \
//                    --bl31 bl31.img.enc \
//                    --bl33 bl33.bin.enc \
//                    --ddrfw1 ddr4_1d.fw \
//                    --ddrfw2 ddr4_2d.fw \
//                    --ddrfw3 ddr3_1d.fw \
//                    --ddrfw4 piei.fw \
//                    --ddrfw5 lpddr4_1d.fw \
//                    --ddrfw6 lpddr4_2d.fw \
//                    --ddrfw7 diag_lpddr4.fw \
//                    --ddrfw8 aml_ddr.fw

#include "types.h"
#include "lib.h"

int main(int argc, char ** argv) {

    opt_values_bootmk_t opt_values;
    char **opt_array_view = (char**)&opt_values;

    option_t opts[21] = {
        { "bl2",  1, 0, 0x30 },       // 0x00
        { "bl30", 1, 0, 0x31 },       // 0x01
        { "bl31", 1, 0, 0x32 },       // 0x02
        { "bl32", 1, 0, 0x33 },       // 0x03
        { "bl33", 1, 0, 0x34 },       // 0x04
        { "bl3x", 1, 0, 0x35 },       // 0x05
        { "output", 1, 0, 0x36 },     // 0x06
        { "userkey", 1, 0, 0x37 },    // 0x07
        { "input", 1, 0, 0x38 },      // 0x08
        { "level", 1, 0, 0x39 },      // 0x09
        { "bl2usb", 1, 0, 0x61 },     // 0x0a
        { "ddrfw0", 1, 0, 0x62 },     // 0x0b
        { "ddrfw1", 1, 0, 0x63 },     // 0x0c
        { "ddrfw2", 1, 0, 0x64 },     // 0x0d
        { "ddrfw3", 1, 0, 0x65 },     // 0x0e
        { "ddrfw4", 1, 0, 0x66 },     // 0x0f
        { "ddrfw5", 1, 0, 0x67 },     // 0x10
        { "ddrfw6", 1, 0, 0x68 },     // 0x11
        { "ddrfw7", 1, 0, 0x69 },     // 0x12
        { "ddrfw8", 1, 0, 0x6a },     // 0x13
        { "ddrfw9", 1, 0, 0x6b }      // 0x14
    };

    uuid__t toc_entry_uuid_list[] = {
        // BL2
        [0] = { { 0xf4, 0x1d, 0x14, 0x86 }, { 0xcb, 0x95 }, { 0xe6, 0x11 },
                  0x84, 0x88, { 0x84, 0x2b, 0x2b, 0x01, 0xca, 0x38 } },
        // BL30
        [1] = { { 0x48, 0x56, 0xcc, 0xc2 }, { 0xcc, 0x85 }, { 0xe6, 0x11 },
                  0xa5, 0x36, { 0x3c, 0x97, 0x0e, 0x97, 0xa0, 0xee } },
        // BL31
        [2] = { { 0xca, 0xaf, 0xb0, 0x33 }, { 0xce, 0x85 }, { 0xe6, 0x11 },
                  0x8c, 0x32, { 0x00, 0x22, 0x19, 0xc7, 0x77, 0x2f } },
        // BL32
        [3] = { { 0x34, 0xa1, 0x48, 0xb8 }, { 0xbc, 0x90 }, { 0xe6, 0x11 },
                  0x8f, 0xef, { 0xa4, 0xba, 0xdb, 0x19, 0xde, 0x03 } },
        // BL33
        [4] = { { 0x8e, 0x59, 0xd6 ,0x5d }, { 0x5e, 0x8b }, { 0xe6, 0x11 },
                  0xbc, 0xb5, { 0xf0, 0xde, 0xf1, 0x83, 0x72, 0x96 } }
    };
    /*
     * These can be found in include/tools_share/firmware_image_package.h
     * from https://github.com/ARM-software/arm-trusted-firmware
     */
    uuid__t toc_payload_uuid_list[] = {
        // UUID_TRUSTED_BOOT_FIRMWARE_BL2
        [0] = { { 0x5f, 0xf9, 0xec, 0x0b }, { 0x4d, 0x22 }, { 0x3e, 0x4d },
                  0xa5, 0x44, { 0xc3, 0x9d, 0x81, 0xc7, 0x3f, 0x0a } },
        // UUID_SCP_FIRMWARE_SCP_BL2
        [1] = { { 0x97, 0x66, 0xfd, 0x3d }, { 0x89, 0xbe }, { 0xe8, 0x49 },
                  0xae, 0x5d, { 0x78, 0xa1, 0x40, 0x60, 0x82, 0x13 } },
        // UUID_EL3_RUNTIME_FIRMWARE_BL31
        [2] = { { 0x47, 0xd4, 0x08, 0x6d }, { 0x4c, 0xfe }, { 0x98, 0x46 },
                  0x9b, 0x95, { 0x29, 0x50, 0xcb, 0xbd, 0x5a, 0x00 } },
        // UUID_SECURE_PAYLOAD_BL32
        [3] = { { 0x05, 0xd0, 0xe1, 0x89 }, { 0x53, 0xdc }, { 0x13, 0x47 },
                  0x8d, 0x2b, { 0x50, 0x0a, 0x4b, 0x7a, 0x3e, 0x38 } },
        // UUID_NON_TRUSTED_FIRMWARE_BL33
        [4] = { { 0xd6, 0xd0, 0xee, 0xa7 }, { 0xfc, 0xea }, { 0xd5, 0x4b },
                  0x97, 0x82, { 0x99, 0x34, 0xf2, 0x34, 0xb6, 0xe4 } }
    };

    bzero(&opt_values, sizeof opt_values);
    int getopt_result = 0;
    while(getopt_long(argc, argv, "", opts, &getopt_result) != -1)
        opt_array_view[getopt_result] = optarg;

    bool_t withUserKey = false;
    char output_file_name[256];
    struct stat stat_info;

    header_t header;
    ddrfw_toc_t ddrfw_toc;

    FILE *fout;

    if(0 == stat(opt_values.bl2, &stat_info)) {
        // rsa_init(&rsa_ctx, 0, 0)
        // if(0 == stat(opt_values.userkey, &stat) &&
        //    0 == aml_gx_load_rsa_key_file(&rsa, opt_values.userkey, opt_values.userkey)) {
        //     withUserKey = true;
        //     // ...
        // }
        if (opt_values.output == NULL) {
            char *suffix = withUserKey ? ".encrypt" : ".pkg";
            sprintf(output_file_name, "%s%s", opt_values.bl2, suffix);
        } else {
            strcpy(output_file_name, opt_values.output);
        }

        unlink(output_file_name);

        fout = fopen(output_file_name, "wb+");
        if(fout != NULL) {
            bzero(&header, 0x4000);
            bzero(&ddrfw_toc, 0x2d0);

            header.fip_magic = 0xaa640001;
            header.serial_number = 0x12345678;
            header.ddrfw_magic = 0x4d464440;
            header.ddrfw_counter = 0x0;

            // header offset.
            uint64_t offset = 0x4000;

            size_t i = 0;

            // iterate over all --ddrfw1 ... --ddrfw9 items.
            for(uint32_t pos = 0xc, j = 0; pos < 0x16; pos++) {
                if(opt_array_view[pos] != NULL &&
                    0 == stat(opt_array_view[pos], &stat_info)) {
                    // printf("Checking %s...\n", opt_array_view[pos]);
                    FILE *fddrfw = fopen(opt_array_view[pos], "rb");
                    // bail if file doesn't exist, or can't be opened.
                    if(fddrfw == NULL) goto end;

                    fseek(fddrfw, 0, SEEK_END);
                    uint64_t size = ftell(fddrfw);
                    // compute size in chunks of 16k, skipping the first
                    // 0x60 bytes.
                    uint64_t size16k = size - 0x60 + 0x3fff & 0xffffc000;

                    ddrfw_toc.entries[j].outBufferPtr
                        = (uchar_t*)calloc(1, size16k + 0x400);

                    if(ddrfw_toc.entries[j].outBufferPtr != NULL) {
                        fseek(fddrfw, 0, SEEK_SET);
                        fread(ddrfw_toc.entries[j].outBufferPtr, 1, size16k, fddrfw);

                        if(*(uint32_t *)(&ddrfw_toc.entries[j].outBufferPtr[0x20]) == 0x4d464440) {

                            uchar_t sha256buf[0x20];
                            sha256(&ddrfw_toc.entries[j].outBufferPtr[0x20], size -0x20, sha256buf);

                            if(0 == memcmp(sha256buf, ddrfw_toc.entries[j].outBufferPtr, 0x20)) {

                                header.ddrfw_counter++;
                                ddrfw_toc.entries[j].size = size16k;

                                memcpy(&header.ddrfw_data[j],
                                       &ddrfw_toc.entries[j].outBufferPtr[0x20],
                                       0x40);
                                header.ddrfw_data[j].size = size16k;
                                header.ddrfw_data[j].offset = offset;
                                sha256(&ddrfw_toc.entries[j].outBufferPtr[0x60],
                                       ddrfw_toc.entries[j].size,
                                       header.ddrfw_data[j].sha256);
                                offset += size16k;
                                j++;
                            }
                        }
                    }
                    fclose(fddrfw);
                }
            }
            // pre-populate the toc_entries.
            for(uint64_t pos = 0; pos < 5; pos++) {
                header.toc_entries[4+pos].offset = 0x188 + pos * 0x468;
                header.toc_entries[4+pos].size = 0x468;
                memcpy(&header.toc_entries[4+pos].uuid, &toc_entry_uuid_list[pos], 0x10);
            }
            fip_entry_header_t default_header[5];
            memset(&default_header, 0, sizeof default_header);
            // we only need to set non-null values, due to memset.
            // BL2
            default_header[0].unk0 = 0x1100000;

            // BL30
            default_header[1].unk0 = 0x10100000;
            default_header[1].x =    0x5000000;
            default_header[1].y =    0x5100000;
            default_header[1].z =    0x300000;
            default_header[1].z2 =   0x200000;
            // BL31
            default_header[2] = (fip_entry_header_t){ 0x5300000, 0x5300000, 0x5300000, 0x2000000, 0x1400000 };
            // BL32
            default_header[3].unk0 = 0x1000000;
            // BL33
            // default_header[4] is null all the way

            // We'll now iterate over the parts: bl2, bl30, bl31, bl32, bl33
            for(size_t partNo = 0; partNo < 5; partNo++) {
                if(0 == stat(opt_array_view[partNo], &stat_info)) {
                    FILE *fin = fopen(opt_array_view[partNo], "rb");
                    if(fin != NULL) {
                        /* file is open! */
                        if(partNo == 0) {
                            /* special handling for bl2 */
                            fseek(fin, 0, SEEK_SET);
                            /* write bl2 from 0x0-0x10000 */
                            file_copy(fin, fout, 0x10000);
                            /* XXX: why do we write the header here?
                             * We'll write it out later again...
                             */
                            fwrite(&header, 1, 0x4000, fout);

                            for(size_t k = 0; k < header.ddrfw_counter; k++) {
                                if(ddrfw_toc.entries[k].size != 0 &&
                                   ddrfw_toc.entries[k].outBufferPtr != NULL) {
                                    fwrite(ddrfw_toc.entries[k].outBufferPtr+0x60,
                                           1,
                                           ddrfw_toc.entries[k].size,
                                           fout);
                                }
                            }
                        } else {
                            /* handle bl30, bl31, bl32, bl33 */
                            off_t part_off = 0x58;
                            off_t payload_off = 0x490;
                            uchar_t buffer[0x100];
                            bzero(buffer, 0x100);
                            fseek(fin, 0, SEEK_SET);
                            fread(buffer, 1, 0x100, fin);

                            /* check if BL3X-HDR */
                            if(0 == memcmp(buffer, "BL3X-HDR", 7) &&
                               *(uint32_t*)&buffer[0xc] == 0x100  &&
                               *(uint32_t*)&buffer[0x30] == 0x12348765) {
                                part_off = 0x158;
                                payload_off = 0x590;
                                memcpy(&default_header[i], &buffer[0x10], 0x20);
                            }
                            if(1 == partNo) {
                                /* special handling for bl30 */
                                payload_off += 0x290;
                            }
                            memcpy(&header.toc_entries[i].uuid, &toc_payload_uuid_list[partNo], 0x10);
                            header.toc_entries[i].offset = offset;
                            header.toc_entries[i].size = stat_info.st_size - payload_off;
                            memcpy(&header.fip_entries[i].header,&default_header[i], sizeof (fip_entry_header_t));
                            // if(withUserKey) {
                            //     ...
                            // }
                            fseek(fin, part_off, SEEK_SET);
                            fread(header.fip_entries[i].payload, 1, 0x40c, fin);
                            fseek(fin, payload_off, SEEK_SET);
                            file_copy(fin, fout, header.toc_entries[i].size);

                            offset += header.toc_entries[i].size;
                            // increment bootloader counter.
                            i++;
                        }
                        fclose(fin);
                    }
                } else {
                    /* could not open the file. */
                    memcpy(&header.toc_entries[i].uuid, &toc_payload_uuid_list[partNo], 0x10);
                    header.toc_entries[i].offset = offset;
                    memcpy(&header.fip_entries[i].header,&default_header[i], sizeof (fip_entry_header_t));
                    if(partNo != 0) {
                        /* if this is not bl2 */
                        /* toc_entries[0..3] are payload
                         *
                         */
                        header.toc_entries[partNo + 3].offset = 0;
                        header.toc_entries[partNo + 3].size   = 0;
                        memset(&header.toc_entries[partNo + 3].uuid,0,0x10);
                    }
                    i++;
                }
            }
            size_t header_size = withUserKey ? 0x3ff0 /* - rsa_ctx.len */ : 0x3fd0;
            uchar_t sha256buf[0x20];
            uchar_t *hdrPtr = (uchar_t*)&header;

            sha256(hdrPtr+0x10, header_size, sha256buf);

            memcpy(header.sha256, sha256buf, 0x20);

            // if(withUserKey) {
            //     ...
            // }
            fseek(fout, 0x10000, SEEK_SET);
            fwrite(&header, 1, 0x4000, fout);

            // if(!withUserKey && opt_values.bl2usb != NULL, ...) {
            //    ...
            // }

            // NOTE: fall through into fclose.
        }
    }
end:
    if(fout != NULL) fclose(fout);
    for(uint64_t pos = 0; pos < 10; pos++)
        if(ddrfw_toc.entries[pos].outBufferPtr != NULL)
            free(ddrfw_toc.entries[pos].outBufferPtr);

    exit(0);
}