INS = /etc/install -n /usr/bin
INSE = /etc/install -n /etc
INSL = /etc/install -f /usr/lib/vpm

all:	vpmc vpmstart vpmstarta vpmtrace vpmsnap

vpmc:	pl pass0 pass1 pass2 pass1a vratfor

pl:
	cd plsrc;\
	make -f pl.mk all;\
	mv pl ..

vratfor:
	cd ratsrc;\
	make -f ratfor.mk all;\
	mv ratfor ../vratfor

vpmstart:

vpmstarta:

vpmtrace:	vpmtrace.o
	cc vpmtrace.o -o vpmtrace
vpmtrace.o:
	cc -c vpmtrace.c
vpmsnap:	vpmsnap.o
	cc vpmsnap.o -o vpmsnap
vpmsnap.o:
	cc -c vpmsnap.c
install:    all
	   $(INSL) pl
	   $(INSL) vratfor
	    cp pass* /usr/lib/vpm
	   $(INS) vpmc
	   $(INSE) vpmtrace
	   $(INSE) vpmsnap
	   $(INSE) vpmstart
	   $(INSE) vpmstarta

clean:
	    -rm -f *.o
		cd ratsrc;\
		make -f ratfor.mk clean
		cd plsrc;\
		make -f pl.mk clobber
clobber:    clean
	    -rm -f pl vpmtrace vpmsnap vratfor

