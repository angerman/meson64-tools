// blx_fix.sh
//
// This is supposed to replicate this:
// blx_fix.sh bl30.bin zero_tmp bl30_zero.bin bl301.bin bl301_zero.bin bl30_new.bin bl30
// blx_fix.sh bl2.bin  zero_tmp bl2_zero.bin  acs.bin   bl21_zero.bin  bl2_new.bin  bl2
//
//      bl2_new.bin                    bl30_new.bin
//    .---------------. -- 0x0000    .---------------. -- 0x0000
//    | bl2.bin~~~~~~ |              | bl30.bin~~~~~ |
//    : ~~~~~~~~~~~~~ :              : ~~~~~~~~~~~~~ :
//    | ~~~0000000000 |              | ~~~0000000000 |
//    | 0000000000000 |              |---------------| -- 0xA000 (41K)
//    |---------------| -- 0xE000    | bl301.bin~~~~ |
//    | acs.bin~~~~~~ |              : ~~~~~~~~~~~~~ :
//    : ~~~~~~~~~~~~~ :              | ~~00000000000 |
//    | ~~~0000000000 |              '---------------' -- 0xD400 (12K)
//    '---------------' -- 0xF000
//
// ACS is probably Amlogic Configurable SPL
//

#include <string.h>

#include "types.h"
#include "lib.h"

int main(int argc, char ** argv) {

    opt_values_pkg_t opt_values;
    char **opt_array_view = (char**)&opt_values;

    option_t opts[2] = {
        { "type",       1, 0, 0x30 },       // 0x00
        { "output",     1, 0, 0x31 }        // 0x01
    };

    bzero(&opt_values, sizeof opt_values);
    int getopt_result = 0;
    while(getopt_long(argc, argv, "", opts, &getopt_result) != -1)
        opt_array_view[getopt_result] = optarg;

    struct stat stat_info;

    // verify we have two files to package.
    if(optind + 2 != argc)
        exit(-EINVAL);

    opt_values.base  = argv[optind];
    opt_values.extra = argv[optind+1];

    if(opt_values.base == NULL || stat(opt_values.base, &stat_info) != 0)
        exit(-ENOINPUT);

    if(opt_values.extra == NULL || stat(opt_values.extra, &stat_info) != 0)
        exit(-ENOINPUT);

    size_t size = 0x0;
    off_t  extra_offset = 0x0;
    if(0 == strncmp(opt_values.type, "bl2", 4)) {
        size         = 0xF000;
        extra_offset = 0xE000;
    } else if (0 == strncmp(opt_values.type, "bl30", 5)) {
        size         = 0xD400;
        extra_offset = 0xA000;
    } else
        exit(-EINVAL);

    uint8_t *buf = calloc(1, size);
    FILE *fbase = fopen(opt_values.base, "rb");
    fread(&buf[0x0], 1, extra_offset, fbase);
    fclose(fbase);
    FILE *fextra = fopen(opt_values.extra, "rb");
    fread(&buf[extra_offset], 1, size - extra_offset, fextra);
    fclose(fextra);
    FILE *fout = fopen(opt_values.output, "wb+");
    fwrite(&buf[0x0], 1, size, fout);
    fclose(fout);
    free(buf);

    exit(0);
}
