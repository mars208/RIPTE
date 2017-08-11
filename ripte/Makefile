export V?=0

CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

.PHONY: all
all:
	$(MAKE) -C test-program CROSS_COMPILE="$(HOST_CROSS_COMPILE)"

.PHONY: clean
clean:
	$(MAKE) -C test-program clean
	$(MAKE) -C measurement-engine clean