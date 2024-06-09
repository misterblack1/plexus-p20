TESTDIR = .
INS = /etc/install
FRC =
CFLAGS = -O
OFILES = arc.o box.o circle.o close.o dot.o erase.o frame.o label.o line.o linmod.o move.o open.o point.o space.o subr.o

all:	libvt0.a

libvt0.a:	$(OFILES)
	ar r $(TESTDIR)/libvt0.a $(OFILES)

install: all
	$(INS) -f /usr/lib $(TESTDIR)/libvt0.a

clean:
	-rm -f *.o 

clobber: clean
	-rm -f $(TESTDIR)/libvt0.a
FRC:
