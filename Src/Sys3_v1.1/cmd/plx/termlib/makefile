CFLAGS=	-O -DCM_N -DCM_GT -DCM_B -DCM_D -I../

libtermlib.a: termcap.o tgoto.o tputs.o
	ar cr libtermlib.a termcap.o tgoto.o tputs.o

clean: 
	-rm -f *.o
clobber: clean
	-rm -f libtermlib.a

install: libtermlib.a
	/etc/install -n /usr/lib libtermlib.a
