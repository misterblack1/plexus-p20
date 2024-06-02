TESTDIR = .
FRC =
INS = /etc/install -n /usr/bin
INSDIR =
CFLAGS = -O
IFLAG = -i

all: dc

dc:
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAG) -o $(TESTDIR)/dc dc.c

dc.o: dc.h $(FRC)

test:
	rtest $(TESTDIR)/dc

install: all
	$(INS) $(TESTDIR)/dc $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/dc

FRC:
