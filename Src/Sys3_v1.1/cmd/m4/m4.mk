TESTDIR = .
YACCRM=-rm
FRC =
INS = /etc/install -n /usr/bin
INSDIR =
CFLAGS = -O

all: m4

m4:	m4.o m4ext.o m4macs.o m4y.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/m4 m4.o m4ext.o m4macs.o m4y.o -ly

m4.o:	m4.c $(FRC)
m4ext.o: m4ext.c $(FRC)
m4macs.o: m4macs.c $(FRC)
m4y.o:	y.tab.c $(FRC)
	$(CC) $(CFLAGS) -c y.tab.c
	mv y.tab.o m4y.o

y.tab.c: m4y.y
	$(YACC) $(YFLAGS) m4y.y

test:
	rtest $(TESTDIR)/m4

install: all
	$(INS) $(TESTDIR)/m4 $(INSDIR)

clean:
	-rm -f *.o
	$(YACCRM) -f y.tab.c 

clobber: clean
	-rm -f $(TESTDIR)/m4

FRC:
