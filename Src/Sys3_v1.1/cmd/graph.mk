TESTDIR = .
FRC =
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin
INS = /etc/install -n $(UBIN)
IFLAG = -i
INSDIR =
CFLAGS = -O
FFLAG =

all: graph

graph: graph.o 
	$(CC) -I$(INCLUDE) $(LDFLAGS) $(FFLAG)  $(IFLAG) -o $(TESTDIR)/graph graph.o -lplot -lm $(LIB)

graph.o:  graph.c $(FRC)

test:
	rtest $(TESTDIR)/graph

install: all
	$(INS) $(TESTDIR)/graph $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/graph

FRC:
