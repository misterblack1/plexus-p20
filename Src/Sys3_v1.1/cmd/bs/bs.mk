TESTDIR = .
FRC =
INS = /etc/install -n /bin
INSDIR =
INCLUDE = /usr/include
CFLAGS = -O -I$(INCLUDE)
FFLAG =
OFILES = atof.o bs.o string.o
IFLAG = -i

all: bs

bs:	$(OFILES)
	$(CC) $(LDFLAGS) $(FFLAG) $(IFLAG) -o $(TESTDIR)/bs $(OFILES) -lm \
	 $(LIB)

atof.o:	atof.c $(FRC)
bs.o:	bs.c $(FRC)
string.o: string.c $(FRC)

test:
	bs testall

install: all
	$(INS) $(TESTDIR)/bs $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/bs

FRC:
