CFLAGS = -DSYS3 -O -c
LDFLAGS = -s
INS=/etc/install

all : more.help more
	
more : more.o regex.o
	cc $(CFLAGS) $(LDFLAGS) more.o regex.o -ltermlib -o more

more.o:\
	$(ROOT)/usr/include/whoami.h\
	$(ROOT)/usr/include/stdio.h\
	$(ROOT)/usr/include/ctype.h\
	$(ROOT)/usr/include/signal.h\
	$(ROOT)/usr/include/errno.h\
	$(ROOT)/usr/include/termio.h\
	$(ROOT)/usr/include/sys/ttold.h\
	$(ROOT)/usr/include/sys/ioctl.h\
	$(ROOT)/usr/include/sgtty.h\
	$(ROOT)/usr/include/setjmp.h\
	$(ROOT)/usr/include/sys/types.h\
	$(ROOT)/usr/include/sys/dir.h\
	$(ROOT)/usr/include/sys/stat.h\
	$(ROOT)/usr/include/local/uparm.h
	cc $(CFLAGS) $<

regex.o:
	cc $(CFLAGS) $<

clean:
	-rm -f *.o

clobber: clean
	rm -f more

install: all
	$(INS) -f /usr/plx more
	$(INS) -f /usr/lib more.help
