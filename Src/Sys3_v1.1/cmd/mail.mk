TESTDIR = .
FRC =
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin
INS = /etc/install
INSDIR = $(UBIN)
CFLAGS = -O
LDFLAGS = -s
INCLUDE = /usr/include

all: mail

mail: mail.c $(FRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(INCLUDE) -o $(TESTDIR)/mail mail.c $(LIB)

install: all
	$(INS) $(TESTDIR)/mail $(BIN)
	-rm -f $(INSDIR)/rmail
	ln $(BIN)/mail $(INSDIR)/rmail

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/mail

FRC:
