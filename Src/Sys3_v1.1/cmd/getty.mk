TESTDIR = .
FRC =
INS = /etc/install
INSDIR = /etc 
LDFLAGS = -s -n
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin

all: getty

getty: getty.o gettytab.o $(FRC)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/getty getty.object gettytab.o $(LIB) 

getty.o: getty.object
	if test -r getty.c ; then $(CC) -c $(CFLAGS) getty.c ; \
		mv getty.o getty.object ; \
	fi
test:
	rtest $(TESTDIR)/getty

install: all
	$(INS) -o $(TESTDIR)/getty $(INSDIR)

clean:
	rm -f getty.o

clobber: clean
	-rm -f $(TESTDIR)/getty

FRC:
