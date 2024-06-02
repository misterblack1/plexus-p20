TESTDIR = .
INS = /etc/install
ULIB = /usr/lib
FRC =
CFLAGS = -O
OFILES = arc.o box.o circle.o close.o dot.o erase.o label.o line.o linmod.o move.o open.o point.o space.o subr.o

all:	lib300s.a

lib300s.a:	$(OFILES)
	ar r $(TESTDIR)/lib300s.a $(OFILES)

install: all
	$(INS) -f /usr/lib $(TESTDIR)/lib300s.a
	-rm -f $(ULIB)/lib300S.a
	ln $(ULIB)/lib300s.a $(ULIB)/lib300S.a

clean:
	-rm -f *.o 

clobber: clean
	-rm -f $(TESTDIR)/lib300s.a
FRC:
