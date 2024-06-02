TESTDIR = .
FRC =
INS = /etc/install -n /usr/bin
INSDIR = 
CFLAGS = -O
OFILES = tp0.o tp1.o tp2.o tp3.o

all: tp

tp: $(OFILES) 
	$(CC) $(LDFLAGS) -o $(TESTDIR)/tp $(OFILES)

$(OFILES): tp.h $(FRC)

test:
	rtest $(TESTDIR)/tp

install: all
	$(INS) $(TESTDIR)/tp $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/tp

FRC:
