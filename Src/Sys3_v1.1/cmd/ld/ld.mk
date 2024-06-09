TESTDIR = .
INS = /etc/install -n /usr/bin
INSDIR =
FRC =
CFLAGS = -O
LDFLAGS= -s -n
all: ld
ld:: ld.vax.c
	-if vax; then $(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/ld ld.vax.c; fi
ld:: ld.pdp.c
	-if pdp11; then $(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/ld ld.pdp.c; fi
ld:: ld.z8000.c
	-if z8000; \
		then $(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/ld ld.z8000.c; \
	fi
ld:: ld.m68.c
	-if m68; \
		then $(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/ld ld.m68.c; \
	fi
ld68::	ld.m68.c
	$(CC) -i -O -DBOOTSTRAP -Dm68 -Uz8000 -o ld68 ld.m68.c
install: all
	$(INS) $(TESTDIR)/ld
clean:
	rm -f *.o
clobber: clean
	rm -f $(TESTDIR)/ld

FRC:
