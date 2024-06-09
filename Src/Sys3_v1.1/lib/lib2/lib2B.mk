all:	Bconf.o Binit.o
	ar rvc lib2B.a Bconf.o Binit.o

install: all
	/etc/install -f /usr/lib lib2B.a

clean:
	rm -f Bconf.o Binit.o

clobber: clean
	rm -f lib2B.a
