
MBEDTLS=mbedtls
LZ4=lz4

CFLAGS = -c -g -I$(MBEDTLS)/include -I$(LZ4)/lib
LDFLAGS = -L$(MBEDTLS)/library -lmbedcrypto -L$(LZ4)/lib -llz4

$(MBEDTLS)/.built:
	$(MAKE) -C $(MBEDTLS) && \
	touch $(MBEDTLS)/.built
$(LZ4)/.built:
	$(MAKE) -C $(LZ4) BUILD_SHARED=no && \
	touch $(LZ4)/.built

%.o : %.c $(MBEDTLS)/.built $(LZ4)/.built
	$(CC) $(CFLAGS) $< -o $@

bootmk: lib.o bootmk.o
	$(CC) $(LDFLAGS) $^ -o $@

bl2sig: lib.o bl2sig.o
	$(CC) $(LDFLAGS) $^ -o $@

bl3sig: lib.o bl3sig.o
	$(CC) $(LDFLAGS) $^ -o $@

bl30sig: lib.o bl30sig.o
	$(CC) $(LDFLAGS) $^ -o $@

pkg: lib.o pkg.o
	$(CC) $(LDFLAGS) $^ -o $@

.phony: all clean install
all: bootmk bl2sig bl3sig bl30sig pkg

clean:
	rm -f $(MBEDTLS)/.built $(LZ4)/.built
	rm -f *.o
	rm -f bl2sig bl30sig bl3sig bootmk

install: bootmk bl2sig bl3sig bl30sig pkg
	install -m 755 $^ $(PREFIX)