CC ?= gcc
CFLAGS ?= -O -Wall

all: elf2coff cofflink

elf2coff: elf2coff.c
	$(CC) $(CFLAGS) -o $@ $<

cofflink: cofflink.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f elf2coff cofflink

.PHONY: all clean
