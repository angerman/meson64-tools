# The Amlogic S922X boot

Some tools to replicate the following:
```
packaging
=========
./aml_encrypt_g12b --bl30sig --input bl30_new.bin	       --output bl30_new.bin.g12a.enc --level v3
./aml_encrypt_g12b --bl3sig  --input bl30_new.bin.g12a.enc --output bl30_new.bin.enc      --level v3 --type bl30
./aml_encrypt_g12b --bl3sig  --input bl31.img              --output bl31.img.enc          --level v3 --type bl31
./aml_encrypt_g12b --bl3sig  --input bl33.bin              --output bl33.bin.enc          --level v3 --type bl33 --compress lz4
./aml_encrypt_g12b --bl2sig  --input bl2_new.bin           --output bl2.n.bin.sig
./aml_encrypt_g12b --bootmk                                --output u-boot.bin            --level v3 \
                   --bl2 bl2.n.bin.sig --bl30 bl30_new.bin.enc --bl31 bl31.img.enc --bl33 bl33.bin.enc \
                   --ddrfw1 ddr4_1d.fw   --ddrfw2 ddr4_2d.fw   --ddrfw3 ddr3_1d.fw     --ddrfw4 piei.fw \
                   --ddrfw5 lpddr4_1d.fw --ddrfw6 lpddr4_2d.fw --ddrfw7 diag_lpddr4.fw --ddrfw8 aml_ddr.fw
```

Clone https://github.com/LibreELEC/amlogic-boot-fip, and put the data files (e.g. `ordroid-n2/*` or `odroid-n2-plus/*`)
into the data-files folder.

Clone https://gitlab.denx.de/u-boot/custodians/u-boot-amlogic.git to build the amlogic `u-boot.bin`
```
git clone https://gitlab.denx.de/u-boot/custodians/u-boot-amlogic.git
nix-shell -p '[ pkgsCross.aarch64-multiplatform.buildPackages.binutils pkgsCross.aarch64-multiplatform.stdenv.cc bison yacc flex bc ]'
make CROSS_COMPILE=aarch64-unknown-linux-gnu- odroid-n2_defconfig
make CROSS_COMPILE=aarch64-unknown-linux-gnu-
```
(See also `./doc/board/amlogic/odroid-n2.rst` in `u-boot-amlogic`).

The `u-boot.bin` will be the `bl33.bin` (see above!).