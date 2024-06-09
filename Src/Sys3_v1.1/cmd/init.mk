TESTDIR = .
FRC =
BIN = /bin
UBIN = /usr/bin
INS = /etc/install
INSDIR = /etc 
CFLAGS = -O
LDFLAGS = -s

all: init

init: init.c $(FRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/init init.c $(LIB)

test:
	rtest $(TESTDIR)/init

install: all
	$(INS) -o $(TESTDIR)/init $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/init

FRC:
