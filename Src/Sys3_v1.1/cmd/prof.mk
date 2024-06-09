TESTDIR = .
FRC =
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin
INS = /etc/install
INSDIR = $(UBIN)
CFLAGS = -O

all: prof

# If you don't want to plot, take out the -Dplot and the ref. to plot.a
prof: prof.c $(FRC)
	$(CC) -I$(INCLUDE) $(LDFLAGS) $(CFLAGS) -o $(TESTDIR)/prof prof.c $(LIB) 

test:
	rtest $(TESTDIR)/prof

install: all
	$(INS) -n $(INSDIR) $(TESTDIR)/prof $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/prof

FRC:
