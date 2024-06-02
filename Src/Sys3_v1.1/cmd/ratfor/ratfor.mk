TESTDIR = .
INS = /etc/install -n /usr/bin
INSDIR =
FRC =
CFLAGS = -O
YACCRM=-rm
LDFLAGS=-s
OFILES = r0.o r1.o r2.o rio.o rlook.o rlex.o

all: ratfor

ratfor:    $(OFILES) r.g.o
	   $(CC) $(OFILES) r.g.o $(LDFLAGS) -o $(TESTDIR)/ratfor -ly

$(OFILES): r.h r.g.h $(FRC)
r.g.c r.g.h:   r.g.y $(FRC)
	   $(YACC) -d r.g.y
	mv y.tab.c r.g.c
	mv y.tab.h r.g.h

test:
	   rtest $(TESTDIR)/ratfor

install:   all
	   $(INS) $(TESTDIR)/ratfor $(INSDIR)

clean:
	   -rm -f *.o
	   $(YACCRM) -f r.g.c r.g.h

clobber:  clean
	   -rm -f $(TESTDIR)/ratfor

FRC:
