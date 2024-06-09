YACCRM=-rm
TESTDIR = .
FRC =
INS = /etc/install -n /usr/lib/vpm
INSDIR =
CFLAGS = -O

all:	pl

pl:	pl.o
	$(CC) $(LDFLAGS) -s -o $(TESTDIR)/pl pl.o -ll
pl.o:	pl.c pl.yl $(FRC)
pl.c:	pl.y $(FRC)
	$(YACC) pl.y
	mv y.tab.c pl.c
pl.yl: pl.l $(FRC)
	lex pl.l
	mv lex.yy.c pl.yl

install:	all
	$(INS)	$(TESTDIR)/pl $(INSDIR)

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(TESTDIR)/pl
	$(YACCRM) -f $(TESTDIR)/pl.yl $(TESTDIR)/pl.c
