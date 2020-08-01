# `blx_fix`

This script is invoked as
```
           $1       $2       $3            $4        $5             $6           $7
blx_fix.sh bl30.bin zero_tmp bl30_zero.bin bl301.bin bl301_zero.bin bl30_new.bin bl30
blx_fix.sh bl2.bin  zero_tmp bl2_zero.bin  acs.bin   bl21_zero.bin  bl2_new.bin  bl2
```
logic is as follows:
```
if arg 7 is bl2 then
    blx_bin_limit=57344   (0xE000)
    blx01_bin_limit=4096  (0x1000)

if arg 7 is bl30 then

    blx_bin_limit=40960   (0xA000)
    blx01_bin_limit=13312 (0x3400)

=> blx_size = bytes of bl2.bin
   zero_size = blx_bin_limit - blx_size

   create zero into zero_tmp of size zero_size

   bl2_zero.bin = bl2.bin filled with zeros till 0xE000

   blx01_size = bytes of acs.bin
   zero_size_01 = blx01_bin_limit - blx01_size

   create zero init op zero_top of size zero_size

   bl21_zero.bin = acs.bin filled with zeros till 0x1000

   And then concat bl2_zero.bin and bl21_zero.bin into bl2_new.bin

     bl2_new.bin                    bl30_new.bin
   .---------------. -- 0x0000    .---------------. -- 0x0000
   | bl2.bin~~~~~~ |              | bl30.bin~~~~~ |
   : ~~~~~~~~~~~~~ :              : ~~~~~~~~~~~~~ :
   | ~~~0000000000 |              | ~~~0000000000 |
   | 0000000000000 |              |---------------| -- 0xA000 (41K)
   |---------------| -- 0xE000    | bl301.bin~~~~ |
   | acs.bin~~~~~~ |              : ~~~~~~~~~~~~~ :
   : ~~~~~~~~~~~~~ :              | ~~00000000000 |
   | ~~~0000000000 |              '---------------' -- 0xD400 (12K)
   '---------------' -- 0xF000

   bl2.bin also has limit of 41K, however the encryption requires 48K, so we'll
   pad up to 0xE000. acs (bl21) has a limit of 3K, we'll pad to 4K, this brings
   the total to 52K.
```

# packaging

```
./aml_encrypt_g12b --bl30sig --input bl30_new.bin	         --output bl30_new.bin.g12a.enc --level v3
./aml_encrypt_g12b --bl3sig  --input bl30_new.bin.g12a.enc --output bl30_new.bin.enc      --level v3 --type bl30
./aml_encrypt_g12b --bl3sig  --input bl31.img              --output bl31.img.enc          --level v3 --type bl31
./aml_encrypt_g12b --bl3sig  --input bl33.bin              --output bl33.bin.enc          --level v3 --type bl33 --compress lz4
./aml_encrypt_g12b --bl2sig  --input bl2_new.bin           --output bl2.n.bin.sig
./aml_encrypt_g12b --bootmk                                --output u-boot.bin            --level v3 \
                   --bl2 bl2.n.bin.sig --bl30 bl30_new.bin.enc --bl31 bl31.img.enc --bl33 bl33.bin.enc \
                   --ddrfw1 ddr4_1d.fw   --ddrfw2 ddr4_2d.fw   --ddrfw3 ddr3_1d.fw     --ddrfw4 piei.fw \
                   --ddrfw5 lpddr4_1d.fw --ddrfw6 lpddr4_2d.fw --ddrfw7 diag_lpddr4.fw --ddrfw8 aml_ddr.fw
```