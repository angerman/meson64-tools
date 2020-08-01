#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <getopt.h>
#include <time.h>
#include <mbedtls/sha256.h>

#define	_UUID_NODE_LEN		6

typedef struct option option_t;
typedef unsigned char uchar_t;
typedef uchar_t sha256_t[0x20];
typedef bool bool_t;

#pragma pack(push, 1)

typedef struct _opt_values_bootmk_t {
    char *bl2;
    char *bl30;
    char *bl31;
    char *bl32;
    char *bl33;
    char *bl3x;
    char *output;
    char *userkey;
    char *input;
    char *level;
    char *bl2usb;
    char *ddrfw0;
    char *ddrfw1;
    char *ddrfw2;
    char *ddrfw3;
    char *ddrfw4;
    char *ddrfw5;
    char *ddrfw6;
    char *ddrfw7;
    char *ddrfw8;
    char *ddrfw9;
} opt_values_bootmk_t;

typedef struct _opt_values_bl3sig_t {
    char *input;
    char *amluserkey;
    char *userkey;
    char *output;
    char *skipsector;
    char *level;
    char *type;
    char *compress;
} opt_values_bl3sig_t;

typedef struct _opt_values_bl30sig_t {
    char *input;
    char *amluserkey;
    char *userkey;
    char *keymax;
    char *aeskey;
    char *output;
    char *efuse;
    char *skipsector;
    char *level;
    char *_pad;
} opt_values_bl30sig_t;

typedef struct _opt_values_pkg_t {
    char *type;
    char *output;
    char *base;
    char *extra;
} opt_values_pkg_t;

typedef struct _uuid_t {
	uint8_t		time_low[4];
	uint8_t		time_mid[2];
	uint8_t		time_hi_and_version[2];
	uint8_t		clock_seq_hi_and_reserved;
	uint8_t		clock_seq_low;
	uint8_t		node[_UUID_NODE_LEN];
} uuid__t;

typedef struct _toc_entry_t {
    uuid__t uuid;
    uint64_t offset;
    uint64_t size;
    uint64_t flags;
} toc_entry_t;

typedef struct _fip_entry_header_t {
    uint64_t unk0;
    uint64_t x;
    uint64_t y;
    uint32_t z;
    uint32_t z2;
} fip_entry_header_t;

typedef struct _fip_entry_t {
    uint64_t unk1;
    fip_entry_header_t header;
    uint32_t encrypted; // ???
    uchar_t aes_key[0x20];
    uint64_t unk2;
    uint64_t unk3;
    uchar_t payload[0x40c];
} fip_entry_t;

typedef struct _ddrfw_data_t {
    uint32_t ddrfw_magic;
    uint32_t unk0;
    uint32_t offset;
    uint32_t size;
    uint64_t unk1;
    uint64_t unk2;
    sha256_t sha256;
} ddrfw_data_t;

typedef struct _ddrfw_entry_t {
    uint32_t size;
    uint32_t unk0[12];
    uchar_t * outBufferPtr;
    uint32_t unk1[3]; // tail pad?
} ddrfw_entry_t;

typedef struct _ddrfw_toc_t {
    uint32_t prefix[3];
    ddrfw_entry_t entries[9];
    uint32_t unk0[15]; // tail pad?
} ddrfw_toc_t;

typedef struct _header_t {
    uint64_t unk0; // zero?
    uint64_t unk1; // zero?
    uint32_t fip_magic;
    uint32_t serial_number;
    uint64_t unk2; // zero pad?
    toc_entry_t toc_entries[9]; // 4 payloads, 5 fip_entries
    fip_entry_t fip_entries[5];
    uint32_t ddrfw_magic;
    uint32_t ddrfw_counter;
    uint64_t unk3; // zero pad?
    ddrfw_data_t ddrfw_data[9];
    uchar_t  unk4[0x2600]; // reserved for future use?
    sha256_t sha256; // digest over the header.
} header_t;

typedef struct _lz4_header_t {
    uint32_t magic;
    uint16_t unk0;
    uint16_t header_size;
    uint32_t original_size;
    uint32_t compressed_size;
    sha256_t file_sha256;
    char     timestamp[32];
    uint8_t  random_nonce[12];
    uint32_t magic_2;
    sha256_t header_sha256; /* sha256 from magic, till 0x60 */
} lz4_header_t;

typedef struct _key_header_t {
    /* 0x00 */ uint32_t magic; // @KEY
    /* 0x04 */ uint32_t size;  // 0x30
    /* 0x08 */ uint8_t  unk0;  // 0x01;
    /* 0x09 */ uint8_t  type;  // ???
    /* 0x0a */ uint8_t  unk1;  // 0x00
    /* 0x0b */ uint8_t  pad0; // 0-pad
    /* 0x0c */ uint32_t name; // ??? R1SA, R2SA, R4SA, R8SA, R1HA, ... based on $type
    /* 0x10 */ sha256_t sha256;
    /* 0x30 */

} key_header_t;

typedef struct _bl3_sig_header_t {
    /* 0x00 */ uint64_t unk0[2];
    /* 0x10 */ uint32_t magic;
    /* 0x14 */ uint32_t unk1;
    /* 0x18 */ uint64_t unk2;
    /* 0x20 */ uint64_t size16k;
    /* 0x28 */ uint64_t size;
    /* 0x30 */ sha256_t file_sha256;
    /* 0x50 */ uint64_t bl_magic;
    /* 0x58 */ uint64_t bl_unk3;
    /* 0x60 */ uint8_t  key[0x30]; // first 0x20, are the last 0x20 of the keyfile. rest 0.
    /* 0x90 */ uchar_t  bl_unk4[480];
    /* 0x270 */ sha256_t sha256;
    /* 0x290 */
} bl3_sig_header_t;

typedef struct _bl30_sig_header_t {
    /* 0x00 */ uint32_t magic; // @AML
    /* 0x04 */ uint32_t total_size;
    /* 0x08 */ uint8_t  hdr_size; // 0x40
    /* 0x09 */ uint8_t  unk0; // ???
    /* 0x0a */ uint8_t  unk1; // 0x01
    /* 0x0b */ uint8_t  unk2; // 0x01
    /* 0x0c */ uint32_t pad0; // padding?
    /* 0x10 */ uint32_t encrypted;
    /* 0x14 */ uint32_t header_size; // 0x40;
    /* 0x18 */ uint32_t block_size;  // 0x200;
    /* 0x1c */ uint32_t sig_body_offset;   // 0x60;
    /* 0x20 */ uint32_t key_len;
    /* 0x24 */ uint32_t key_offset;
    /* 0x28 */ uint32_t payload2_with_key;
    /* 0x2c */ uint32_t body_size;
    /* 0x30 */ uint32_t pad1; // 0-padding
    /* 0x34 */ uint32_t payload_offset;
    /* 0x38 */ uint32_t input_size16k;
    /* 0x3c */ uint32_t pad2; // 0-padding
    /* 0x40 */
} bl30_sig_header_t;

enum _error_codes {
    ENOINPUT = 0x1000, // MAX_ERRNO+1 on linux
    INPUTBADSIZE,
    EKEY,
    EKEYBADSIZE,
    EKEYBAD,
};

#pragma pack(pop)
