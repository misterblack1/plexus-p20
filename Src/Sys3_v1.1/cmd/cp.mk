TESTDIR = .
FRC =
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin
INS = /etc/install -n $(BIN)
INSDIR = 
CFLAGS = -O
LDFLAGS = -n -s

all: mv

mv: mv.c $(FRC)
	$(CC) -I$(INCLUDE) $(LDFLAGS) -o $(TESTDIR)/mv $(CFLAGS) mv.c $(LIB)

test:
	rtest $(TESTDIR)/mv

install: all
	cp $(TESTDIR)/mv $(TESTDIR)/ln
	PATH=$(TESTDIR):$(BIN):$(UBIN):/etc $(INS) $(TESTDIR)/mv $(INSDIR)
	rm -f $(BIN)/ln
	PATH=$(TESTDIR):$(BIN):$(UBIN):/etc $(INS) $(TESTDIR)/ln $(INSDIR)
	./ln $(BIN)/ln $(BIN)/cp

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/mv $(TESTDIR)/ln

FRC:
