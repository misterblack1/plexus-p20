INCRT = $(ROOT)/usr/include
CFLAGS = -O -I$(INCRT)
FRC =

FILES =\
	icpdmp.o

all:	$(FILES)

clean:
	-rm -f *.o

clobber:	clean
	-rm -f icpdmp

icpdmp:	icpdmp.c
	cc $(CFLAGS) icpdmp.c -o icpdmp
	cp icpdmp $(ROOT)/sioc
	$(FRC)
