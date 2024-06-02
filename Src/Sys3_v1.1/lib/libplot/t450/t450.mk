TESTDIR = .
INS = /etc/install
FRC =
CFLAGS = -O
OFILES = arc.o box.o circle.o close.o dot.o erase.o label.o line.o linmod.o move.o open.o point.o space.o subr.o

all:	lib450.a

lib450.a:	$(OFILES)
	ar r $(TESTDIR)/lib450.a $(OFILES)

install: all
	$(INS) -f /usr/lib $(TESTDIR)/lib450.a

clean:
	-rm -f *.o 

clobber: clean
	-rm -f $(TESTDIR)/lib450.a
FRC:
