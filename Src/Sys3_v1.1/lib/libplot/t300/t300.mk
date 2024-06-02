TESTDIR = .
INS = /etc/install
FRC =
CFLAGS = -O
OFILES = arc.o box.o circle.o close.o dot.o erase.o label.o line.o linmod.o move.o open.o point.o space.o subr.o

all:	lib300.a

lib300.a:	$(OFILES)
	ar r $(TESTDIR)/lib300.a $(OFILES)

install: all
	$(INS) -f /usr/lib $(TESTDIR)/lib300.a

clean:
	-rm -f *.o 

clobber: clean
	-rm -f $(TESTDIR)/lib300.a
FRC:
