TESTDIR = .
INS = /etc/install -n /usr/bin
CFLAGS = -O
IFLAG = -i
FRC =
INSDIR =
FILES = main.o sub1.o sub2.o header.o

all: itest lex

itest:
#	REMOVE # FROM FOLLOWING WHEN R-III MAKE, IF, SH, ETC. INSTALLED.
#	@if [ x$(IFLAG) != x-i ] ; then echo NO ID SPACE ; exit 1 ; else exit 0 ; fi

lex:	$(FILES) y.tab.o
	$(CC) $(LDFLAGS) $(IFLAG) $(FILES) y.tab.o -ly -o $(TESTDIR)/lex

$(FILES): ldefs.c $(FRC)
main.o:	  once.c $(FRC)
y.tab.c:  parser.y $(FRC)
	  $(YACC) parser.y

test:
	  rtest $(TESTDIR)/lex

install: all
	 $(INS) $(TESTDIR)/lex $(INSDIR)

clean:
	 -rm -f *.o y.tab.c

clobber: clean
	 -rm -f $(TESTDIR)/lex

FRC:
