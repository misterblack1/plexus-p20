TESTDIR = .
FRC =
INS = /etc/install
INSDIR =  /etc
CFLAGS = -O -DSYS3
#LDFLAGS = -s

OFILES = buf.o callout.o file.o inode.o main.o misc.o mount.o proc.o \
		search.o stat.o text.o tty.o u.o
crash:	$(OFILES) cmd.h crash.h 
	$(CC) $(CFLAGS) $(FFLAG) $(LDFLAGS) -o $(TESTDIR)/crash $(OFILES)

install: crash
	$(INS) -n /etc $(TESTDIR)/crash $(INSDIR)

clean:
	-rm -f *.o

clobber: clean
	-rm -f $(TESTDIR)/crash

FRC:
