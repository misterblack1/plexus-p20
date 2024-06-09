TESTDIR = .
FRC =
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin
INS = /etc/install
IFLAG = -i
LDFLAGS = -s -n
INSDIR = -n $(UBIN)
CFLAGS = -O
FFLAG =
SMFLAG=

all: ncheck

ncheck: ncheck.c 
	-if [ x$(IFLAG) != x-i ]  ; then \
	$(CC) -I$(INCLUDE) $(LDFLAGS) -Dsmall=-1 $(CFLAGS) $(IFLAG) -o $(TESTDIR)/ncheck ncheck.c $(LIB) ; \
	else $(CC) -I$(INCLUDE) $(LDFLAGS) $(CFLAGS) $(IFLAG) -o $(TESTDIR)/ncheck ncheck.c $(LIB) ; \
	fi


test:
	rtest $(TESTDIR)/ncheck

install: all
	$(INS) $(INSDIR) $(TESTDIR)/ncheck /etc

clean:

clobber: clean
	-rm -f $(TESTDIR)/ncheck

FRC:
