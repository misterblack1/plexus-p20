TESTDIR = .
FRC =
INS = /etc/install -n /bin
INSDIR =
CFLAGS = -O
OFILES = sed0.o sed1.o

all: sed

sed:	$(OFILES)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(TESTDIR)/sed $(OFILES)

$(OFILES): sed.h $(FRC)

test:
	rtest $(TESTDIR)/sed

install:	all
		$(INS) $(TESTDIR)/sed $(INSDIR)

clean:
	-rm -f *.o

clobber:	clean
		-rm -f $(TESTDIR)/sed

FRC:
