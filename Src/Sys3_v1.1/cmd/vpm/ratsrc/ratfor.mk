TESTDIR = .
INS = /etc/install -n /usr/bin
INSDIR =
FRC =
CFLAGS = -O
YACCRM=-rm
OFILES = r0.o r1.o r2.o rio.o rlook.o rlex.o

all: ratfor

ratfor:    $(OFILES) y.tab.o
	   $(CC) $(OFILES) y.tab.o -s -o $(TESTDIR)/ratfor -ly

$(OFILES): r.h y.tab.h $(FRC)
y.tab.c:   r.g $(FRC)
	   yacc -d r.g
y.tab.h:   r.g $(FRC)
	   yacc -d r.g

test:
	   rtest $(TESTDIR)/ratfor

install:   all
	   $(INS) $(TESTDIR)/ratfor $(INSDIR)

clean:
	   -rm -f *.o
	   $(YACCRM) -f y.tab.c y.tab.h

clobber:  clean
	   -rm -f $(TESTDIR)/ratfor

FRC:
