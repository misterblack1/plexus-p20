TESTDIR = .
FRC =
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin
INS = /etc/install
INSDIR = /etc 
CFLAGS = -O
LDFLAGS = -s

all: login

login: login.c $(FRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(INCLUDE) -o $(TESTDIR)/login login.c $(LIB)

test:
	rtest $(TESTDIR)/login

install: all
	$(INS) -o $(TESTDIR)/login $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/login

FRC:
