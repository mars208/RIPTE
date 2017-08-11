CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

CFLAGS += -Wall -I../include -I$(TEEC_EXPORT)/include -I$(TA_DEV_KIT_DIR)/host_include -marm
LDADD += -lteec -lm -L$(TEEC_EXPORT)/lib -marm

BINARY = sample
OBJS = ripte_helpers.o ripte_stub.o trampoline.o btbl.o ltbl.o ftbl.o fdtbl.o list.o \
	basicmath/isqrt.o basicmath/rad2deg.o basicmath/cubic.o basicmath/basicmath_small.o basicmath/basicmath_large.o \
	sample.o

	# CRC32/crc_32.o \
	# dijkstra/dijkstra_large.o dijkstra/dijkstra_small.o \
	# sha/sha_driver.o sha/sha.o \
	# stringsearch/pbmsrch_large.o stringsearch/pbmsrch_small.o \
	# jpeg/jpeg-6a/cjpeg.o jpeg/jpeg-6a/djpeg.o jpeg/jpeg-6a/rdppm.o jpeg/jpeg-6a/rdgif.o jpeg/jpeg-6a/rdtarga.o \
	# jpeg/jpeg-6a/rdrle.o jpeg/jpeg-6a/rdbmp.o jpeg/jpeg-6a/rdswitch.o jpeg/jpeg-6a/wrppm.o jpeg/jpeg-6a/wrgif.o \
	# jpeg/jpeg-6a/wrtarga.o jpeg/jpeg-6a/wrrle.o jpeg/jpeg-6a/wrbmp.o jpeg/jpeg-6a/rdcolmap.o \
	# jpeg/jpeg-6a/cdjpeg.o jpeg/jpeg-6a/libjpeg.a \

.PHONY: all instrumentation jpeg
all: instrumentation clean_obj

.PHONY: instrumentation
instrumentation: jpeg $(BINARY)
	cp $(BINARY) ./backup/latest_backup
	./instrumentation/hookit --print-cfs-table --print-branch-table --print-loop-table --create-branch-table --create-loop-table
	make $(BINARY)
	./instrumentation/hookit #--dry-run

jpeg:
	make -C jpeg/jpeg-6a 

$(BINARY): $(OBJS)
	$(CC) $(LDADD) -o $@ $^

%.o: %.c
	$(q)$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS) $(BINARY)

.PHONY: clean_obj
clean_obj:
	rm -f $(OBJS)
