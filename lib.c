#include "types.h"

int sha256n(size_t nbuf, uchar_t *buf[], size_t size[], uchar_t *digest_buf) {
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0); // 0 means use sha256.
    for(size_t i = 0; i < nbuf; i++) {
        mbedtls_sha256_update_ret(&ctx, buf[i], size[i]);
    }
    mbedtls_sha256_finish_ret(&ctx, digest_buf);
    mbedtls_sha256_free(&ctx);
    return 0;
}
int sha256(uchar_t *buf, size_t size, uchar_t *digest_buf) {
    return sha256n(1, &buf, &size, digest_buf);
}

int file_sha256(char *fname, uchar_t *digest_buf) {
    struct stat stat_info;
    if(NULL == fname ||
       NULL == digest_buf ||
       0 != stat(fname, &stat_info))
       return 1;

    FILE *fin = fopen(fname,"rb");
    if(fin == NULL) {
        fclose(fin);
        return 2;
    }

    off_t offset = ftell(fin);
    fseek(fin, 0, SEEK_END);
    size_t eof = ftell(fin);
    size_t size = eof - offset;
    if((size & 0xf) != 0) {
        fclose(fin);
        return 3;
    }
    fseek(fin, 0, SEEK_SET);

    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0); // 0 means use sha256.

    void *buf = malloc(0x10000);
    size_t buf_size = 0;
    while(size > 0) {
        if(size < 0x10001) {
            buf_size = fread(buf, 1, size, fin);
        } else {
            buf_size = fread(buf, 1, 0x10000, fin);
        }
        mbedtls_sha256_update_ret(&ctx, buf, buf_size);
        size -= buf_size;
    }
    mbedtls_sha256_finish_ret(&ctx, digest_buf);
    mbedtls_sha256_free(&ctx);

    fclose(fin);
    return 0;
}

int file_copy(FILE *fin, FILE *fout, size_t size) {
    void * buffer = calloc(1, 0x400);
    int ret = 1;
    if(NULL != fin && NULL != fout && size > 0) {
        off_t fin_offset = ftell(fin);
        fseek(fin,0,SEEK_END);
        size_t fin_size = ftell(fin);

        if(size <= fin_size - fin_offset) {
            fseek(fin, fin_offset, SEEK_SET);
            while(size > 0) {
                size_t len = size < 0x401 ? size : 0x400;

                if( 0 == fread(buffer, 1, len, fin)) break;
                fwrite(buffer, 1, len, fout);

                size -= len;
            }
            ret = 0;
        }
    }
    free(buffer);
    return ret;
}

int file_extend(char *fname, size_t target_size, uint8_t fill) {
    void *buf = malloc(0x400);
    struct stat stat_info;

    if(fname != NULL) {
        if(0 == stat(fname, &stat_info)) {
            FILE *f = fopen(fname, "rb+");
            if(f != NULL) {
                fseek(f, 0, SEEK_END);
                size_t eof = ftell(f);
                if(target_size > eof) {
                    size_t delta = target_size - eof;
                    memset(buf, fill, 0x400);
                    while(delta > 0) {
                        if(delta < 0x401) {
                            fwrite(buf, 1, delta, f);
                            delta -= delta;
                        } else {
                            fwrite(buf, 1, 0x400, f);
                            delta -= 0x400;
                        }
                    }
                }
                fclose(f);
            }
        }
    }
    free(buf);

    return 0;
}
int file_append(char *src, char *dst) {
    struct stat stat_info_src;
    struct stat stat_info_dst;

    FILE *fsrc = NULL;
    FILE *fdst = NULL;

    int ret = 1;

    if(0 == stat(dst, &stat_info_dst) &&
       0 == stat(src, &stat_info_src) &&
       (fsrc = fopen(src, "rb"), fsrc != NULL) &&
       (fdst = fopen(dst, "rb+"), fdst != NULL)) {
        fseek(fdst, 0, SEEK_END);
        ret = file_copy(fsrc, fdst, stat_info_src.st_size);
    }
    if(fdst != NULL) fclose(fdst);
    if(fsrc != NULL) fclose(fsrc);

    return ret;
}