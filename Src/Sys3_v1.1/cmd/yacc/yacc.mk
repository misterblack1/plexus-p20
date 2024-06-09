TESTDIR = .
FRC =
INS = /etc/install
INSDIR =
CFLAGS = -O
IFLAG = -i
OFILES = y1.o y2.o y3.o y4.o

all: itest yacc

itest:
	-if [ x$(IFLAG) != x-i ] ; then echo NO ID SPACE ; exit 1 ; else exit 0 ; fi

yacc:	$(OFILES)
	$(CC) $(LDFLAGS) $(IFLAG) -o $(TESTDIR)/yacc $(OFILES)

$(OFILES): dextern files $(FRC)

test:
	   rtest $(TESTDIR)/yacc

install:   all
	   $(INS) -n /usr/bin $(TESTDIR)/yacc $(INSDIR)
	   $(INS) -n /usr/lib $(TESTDIR)/yaccpar

clean:
	   -rm -f *.o

clobber:   clean
	   -rm -f $(TESTDIR)/yacc

FRC:
