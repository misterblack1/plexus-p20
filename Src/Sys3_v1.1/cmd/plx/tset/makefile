CFLAGS = -O -DSYS3

FILES = tset.o

all: $(FILES)

tset.o:\
	$(ROOT)/usr/include/ctype.h\
	$(ROOT)/usr/include/termio.h\
	$(ROOT)/usr/include/sgtty.h\
	$(ROOT)/usr/include/stdio.h\
	tset.delays.h
	cc $(CFLAGS) $< -ltermlib -o tset

install:
	chown bin tset
	chgrp bin tset
	chmod 755 tset
	install -f /usr/plx tset

clean:
	-rm -f *.o

clobber:
	-rm -f *.o
	-rm -f tset
