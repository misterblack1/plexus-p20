TESTDIR = .
INS = /etc/install
FRC =
CFLAGS = -O
OFILES = arc.o box.o circle.o close.o dot.o erase.o label.o line.o linemod.o move.o open.o point.o scale.o space.o subr.o

all:	lib4014.a

lib4014.a:	$(OFILES)
	ar r $(TESTDIR)/lib4014.a $(OFILES)

install: all
	$(INS) -f /usr/lib $(TESTDIR)/lib4014.a

clean:
	-rm -f *.o 

clobber: clean
	-rm -f $(TESTDIR)/lib4014.a
FRC:
