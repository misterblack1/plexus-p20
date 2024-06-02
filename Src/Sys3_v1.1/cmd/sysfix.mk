TESTDIR = .
FRC =
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin
INS = /etc/install
INSDIR = /etc 
CFLAGS = -O
LDFLAGS = -s

all: stest sysfix

sysfix: sysfix.c $(FRC)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LDFLAGS) -o $(TESTDIR)/sysfix sysfix.c \
		$(LIB) 

stest:
	: set +e; : if test -f vax;: then echo "\nNO SYSFIX on VAX\n";: exit 1 \
	;: fi

test:
	rtest $(TESTDIR)/sysfix

install: all
	$(INS) $(TESTDIR)/sysfix $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/sysfix

FRC:
