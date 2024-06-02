TESTDIR = .
FRC =
INS = /etc/install -n /usr/bin
INSDIR =
IFLAG=-i
OBJECTS = sno1.o sno2.o sno3.o sno4.o
CFLAGS = -O

all:	sno

sno:	$(OBJECTS)
	$(CC) $(LDFLAGS) $(IFLAG) -o $(TESTDIR)/sno $(OBJECTS)

$(OBJECTS):	sno.h $(FRC)

install:	all
	$(INS) $(TESTDIR)/sno $(INSDIR)

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(TESTDIR)/sno

FRC:
