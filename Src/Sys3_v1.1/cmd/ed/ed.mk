TESTDIR = .
FRC =
INS = /etc/install -o
INSDIR = 
CFLAGS = -O

all: ed

ed: ed.s
	$(CC) $(LDFLAGS) -o $(TESTDIR)/ed ed.s 

ed.s:	ed.c $(FRC)
	$(CC) -S $(CFLAGS) -DPWB ed.c
	./edfun ed.s

test:
	rtest $(TESTDIR)/ed

install: all
	$(INS) $(TESTDIR)/ed $(INSDIR)
	-rm -f /bin/red
	ln /bin/ed /bin/red

clean:
	-rm -f ed.s

clobber: clean
	-rm -f $(TESTDIR)/ed

FRC:
