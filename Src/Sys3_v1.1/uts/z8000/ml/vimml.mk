
LIBNAME = ../vimlib0
LOCORE = ../locore.o
INCRT = $(ROOT)/usr/include
CFLAGS = -O -I$(INCRT)
FRC =

AFILES = \
	$(LIBNAME)(fpinst.o)\
	$(LIBNAME)(softfp.o)

OFILES =start.o trap.o copy.o bufio.o clist.o math.o misc.o cswitch.o\
	csubr.o userio.o end.o

all:	$(LIBNAME) $(LOCORE)

.PRECIOUS:	$(LIBNAME) 

$(LIBNAME):	$(AFILES) $(FRC)
		strip $(LIBNAME)

$(LOCORE):	$(OFILES) $(FRC)
	-ld -r -x -o $(LOCORE) $(OFILES)

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME)
	-rm -f $(LOCORE)

print:
	lnum ml.mk ../ml/*.s > /dev/lp

.s.a:
	/lib/cpp -D$(HWFP) -I$(INCRT) -P $< >tempfile
	as -u tempfile -o $%
	ar rv $@ $*.o
	rm -f $*.o tempfile

.s.o:
	/lib/cpp -D$(HWFP) -I$(INCRT) -P $< >tempfile
	as -u tempfile -o $@
	rm tempfile

$(LIBNAME)(fpinst.o):\
	$(INCRT)/sys/plexus.h\
	$(FRC)

$(LIBNAME)(softfp.o):\
	$(FRC)

bufio.o:\
	$(INCRT)/sys/plexus.h

copy.o:\
	$(INCRT)/sys/plexus.h

csubr.o:\
	$(INCRT)/sys/plexus.h

misc.o:\
	$(INCRT)/sys/plexus.h

start.o:\
	$(INCRT)/sys/plexus.h

trap.o:\
	$(INCRT)/sys/plexus.h

userio.o:\
	$(INCRT)/sys/plexus.h

FRC:
