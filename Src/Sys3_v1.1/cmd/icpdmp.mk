INCRT = $(ROOT)/usr/include
CFLAGS = -O  -I$(INCRT)
FRC =

FILES =\
	icpdmp.o

all:	$(FILES)

clean:
	-rm -f icpdmp.o

clobber:	clean
	-rm -f icpdmp

icpdmp:	icpdmp.c
	cc $(CFLAGS) icpdmp.c -o icpdmp
	/etc/install -n /etc icpdmp
	$(FRC)
