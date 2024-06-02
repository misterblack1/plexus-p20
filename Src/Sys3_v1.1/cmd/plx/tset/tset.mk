CFLAGS = -O -DSYS3
INS=/etc/install
FILES = tset

all: $(FILES)

tset tset.o:\
	$(ROOT)/usr/include/ctype.h\
	$(ROOT)/usr/include/termio.h\
	$(ROOT)/usr/include/sgtty.h\
	$(ROOT)/usr/include/stdio.h\
	tset.delays.h
	cc $(CFLAGS) tset.c -ltermlib -o tset

install: all
	$(INS) -f /usr/plx tset

clean:
	-rm -f *.o

clobber: clean
	-rm -f tset
