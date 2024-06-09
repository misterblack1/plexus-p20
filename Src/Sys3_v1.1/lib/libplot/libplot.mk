TESTDIR = .
INS = :
FRC =
ULIB = /usr/lib

all: lib300.a lib300s.a lib4014.a lib450.a libplot.a vt0.a

lib300.a:
	cd t300; make -f t300.mk install INS=$(INS)  \
	ULIB=$(ULIB)

lib300s.a:
	cd t300s; make -f t300s.mk install INS=$(INS)   \
	ULIB=$(ULIB)

lib4014.a:
	cd t4014; make -f t4014.mk install INS=$(INS)  \
	ULIB=$(ULIB)

lib450.a:
	cd t450; make -f t450.mk install INS=$(INS)  \
	ULIB=$(ULIB)

libplot.a:
	cd plot; make -f plot.mk install INS=$(INS)  \
	ULIB=$(ULIB)

vt0.a:
	cd vt0; make -f vt0.mk install INS=$(INS)  \
	ULIB=$(ULIB)

install:
	make -f libplot.mk all INS=/etc/install  ULIB=$(ULIB)

clobber:
	cd vt0; make -f vt0.mk clobber
	cd plot; make -f plot.mk clobber
	cd t450; make -f t450.mk clobber
	cd t4014; make -f t4014.mk clobber
	cd t300s;make -f t300s.mk clobber
	cd t300;make -f t300.mk clobber

FRC:
