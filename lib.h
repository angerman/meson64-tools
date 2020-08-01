#include "types.h"
int sha256n(size_t nbuf, uchar_t *buf[], size_t size[], uchar_t *digest_buf);
int sha256(uchar_t * buf, size_t size, uchar_t *digest_buf);
int file_sha256(char *fname, uchar_t *digest_buf);
int file_copy(FILE *fin, FILE *fout, size_t size);
int file_extend(char *fname, size_t target_size, uint8_t fill);
int file_append(char *src, char *dst);