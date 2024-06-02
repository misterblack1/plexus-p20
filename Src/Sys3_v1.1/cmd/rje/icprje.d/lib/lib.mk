CC= cc
CCFLAGS= -O -Dz8000 -DVPMSYS
FILES= reboot.o getlines.o sysname.o name.o \
	rjedead.o copy.o nmatch.o

install:	rje.a
.c.o:
	$(CC) -c $(CCFLAGS) $<

rje.a:	$(FILES)
	-rm rje.a
	ar r rje.a $(FILES)

reboot.o:	rjedead.o

clean:
	-rm -f *.o
