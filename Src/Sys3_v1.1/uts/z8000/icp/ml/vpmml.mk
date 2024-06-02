INCRT = $(ROOT)/usr/include
CFLAGS = -O -DICP -Dz8000 -DVPMSYS -DVPMICP -I$(INCRT)
FRC =

FILES =\
	vpmclist.o

all:	$(FILES)

vpmclist.o:\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/tty.h
	cc $(CFLAGS) -c $<
	$(FRC)

print:
	lnum ../ml/vpm*.c > /dev/lp

FRC:
