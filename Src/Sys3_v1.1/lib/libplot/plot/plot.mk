TESTDIR = .
INS = /etc/install
FRC =
CFLAGS = -O
OFILES = arc.o box.o circle.o close.o cont.o dot.o erase.o label.o line.o linmod.o move.o open.o point.o putsi.o space.o

all:	libplot.a

libplot.a:	$(OFILES)
	ar r $(TESTDIR)/libplot.a $(OFILES)

install: all
	$(INS) -f /usr/lib $(TESTDIR)/libplot.a

clean:
	-rm -f *.o 

clobber: clean
	-rm -f $(TESTDIR)/libplot.a
FRC:
