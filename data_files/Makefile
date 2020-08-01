# Let's see if we can replicate this.
#
# blx_fix.sh bl30.bin zero_tmp bl30_zero.bin bl301.bin bl301_zero.bin bl30_new.bin bl30
# blx_fix.sh bl2.bin  zero_tmp bl2_zero.bin  acs.bin   bl21_zero.bin  bl2_new.bin  bl2

# ./aml_encrypt_g12b --bl30sig --input bl30_new.bin	         --output bl30_new.bin.g12a.enc --level v3
# ./aml_encrypt_g12b --bl3sig  --input bl30_new.bin.g12a.enc --output bl30_new.bin.enc      --level v3 --type bl30
# ./aml_encrypt_g12b --bl3sig  --input bl31.img              --output bl31.img.enc          --level v3 --type bl31
# ./aml_encrypt_g12b --bl3sig  --input bl33.bin              --output bl33.bin.enc          --level v3 --type bl33 --compress lz4
# ./aml_encrypt_g12b --bl2sig  --input bl2_new.bin           --output bl2.n.bin.sig
# ./aml_encrypt_g12b --bootmk                                --output u-boot.bin            --level v3 \
#                    --bl2 bl2.n.bin.sig --bl30 bl30_new.bin.enc --bl31 bl31.img.enc     --bl33 bl33.bin.enc \
#                    --ddrfw1 ddr4_1d.fw   --ddrfw2 ddr4_2d.fw   --ddrfw3 ddr3_1d.fw     --ddrfw4 piei.fw \
#                    --ddrfw5 lpddr4_1d.fw --ddrfw6 lpddr4_2d.fw --ddrfw7 diag_lpddr4.fw --ddrfw8 aml_ddr.fw
.phony: clean
clean:
	rm -f bl30_new.bin bl2_new.bin

	rm -f bl30_new.bin.g12a.enc bl30_new.bin.enc
	rm -f bl31.img.enc bl33.bin.enc
	rm -f bl2.n.bin.sig u-boot.bin

	rm -f bl30_new.bin.pkg bl30_new.bin.pkg.pkg
	rm -f bl31.img.pkg bl33.bin.pkg
	rm -f bl2_new.bin.pkg.pkg bl2_new.bin.pkg

	rm -f *.hd

bl30_new.bin: bl30.bin bl301.bin
	./blx_fix.sh bl30.bin zero_tmp bl30_zero.bin bl301.bin bl301_zero.bin bl30_new.bin bl30

bl2_new.bin: bl2.bin acs.bin
	./blx_fix.sh bl2.bin  zero_tmp bl2_zero.bin  acs.bin   bl21_zero.bin  bl2_new.bin  bl2

bl30_new.bin.g12a.enc: bl30_new.bin
	./aml_encrypt_g12b --bl30sig --input bl30_new.bin	       --output bl30_new.bin.g12a.enc --level v3

bl30_new.bin.enc: bl30_new.bin.g12a.enc
	./aml_encrypt_g12b --bl3sig  --input bl30_new.bin.g12a.enc --output bl30_new.bin.enc      --level v3 --type bl30

bl31.img.enc: bl31.img
	./aml_encrypt_g12b --bl3sig  --input bl31.img              --output bl31.img.enc          --level v3 --type bl31

bl33.bin.enc: bl33.bin
	./aml_encrypt_g12b --bl3sig  --input bl33.bin              --output bl33.bin.enc          --level v3 --type bl33
	# --compress lz4

bl2.n.bin.sig: bl2_new.bin
	./aml_encrypt_g12b --bl2sig  --input bl2_new.bin           --output bl2.n.bin.sig

u-boot.bin: bl2.n.bin.sig bl30_new.bin.enc bl31.img.enc bl33.bin.enc
	./aml_encrypt_g12b --bootmk                                --output u-boot.bin            --level v3 \
	               --bl2 bl2.n.bin.sig --bl30 bl30_new.bin.enc --bl31 bl31.img.enc     --bl33 bl33.bin.enc \
	               --ddrfw1 ddr4_1d.fw   --ddrfw2 ddr4_2d.fw   --ddrfw3 ddr3_1d.fw     --ddrfw4 piei.fw \
	               --ddrfw5 lpddr4_1d.fw --ddrfw6 lpddr4_2d.fw --ddrfw7 diag_lpddr4.fw --ddrfw8 aml_ddr.fw

# this is the u-boot.bin file name if we don't provide --output
bl2_new.bin.pkg.pkg: bl2_new.bin.pkg bl30_new.bin.pkg.pkg
	../bootmk --bl2    bl2_new.bin.pkg \
			  --bl30   bl30_new.bin.pkg.pkg \
			  --bl31   bl31.img.pkg \
			  --bl33   bl33.bin.pkg \
			  --ddrfw1 ddr4_1d.fw \
			  --ddrfw2 ddr4_2d.fw \
			  --ddrfw3 ddr3_1d.fw \
			  --ddrfw4 piei.fw \
			  --ddrfw5 lpddr4_1d.fw \
			  --ddrfw6 lpddr4_2d.fw \
			  --ddrfw7 diag_lpddr4.fw \
			  --ddrfw8 aml_ddr.fw

bl30_new.bin.pkg: bl30_new.bin
	../bl30sig --input $<

bl30_new.bin.pkg.pkg: bl30_new.bin.pkg
	../bl3sig --input $<

bl31.img.pkg: bl31.img
	../bl3sig --input $<

bl33.bin.pkg: bl33.bin
	../bl3sig --input $<
	# --compress lz4

bl2_new.bin.pkg: bl2_new.bin
	../bl2sig --input $<

%.hd: %
	hexdump -C $< > $@

bl30_new.bin.g12a.diff: bl30_new.bin.g12a.enc.hd bl30_new.bin.pkg.hd
	diff -u $^ || true

bl30_new.bin.enc.diff: bl30_new.bin.enc.hd bl30_new.bin.pkg.pkg.hd
	diff -u $^ || true

bl31.img.enc.diff: bl31.img.enc.hd bl31.img.pkg.hd
	diff -u $^ || true

bl33.bin.enc.diff: bl33.bin.enc.hd bl33.bin.pkg.hd
	diff -u $^ || true

bl2.n.bin.sig.diff: bl2.n.bin.sig.hd bl2_new.bin.pkg.hd
	diff -u $^ || true

.phony: all.diff
all.diff: bl30_new.bin.g12a.diff bl30_new.bin.enc.diff bl31.img.enc.diff bl33.bin.enc.diff bl2.n.bin.sig.diff