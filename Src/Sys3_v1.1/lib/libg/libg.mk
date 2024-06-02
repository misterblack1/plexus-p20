INS=/etc/install
TESTDIR=.
all: test libg.a

test:	
	@set +e;if pdp11;then echo "Sdb does not run on the pdp11";exit 1;fi

libg.a: dbxxx.s
	as -o $(TESTDIR)/libg.a dbxxx.s

install: all
	$(INS) -n /usr/lib $(TESTDIR)/libg.a

clean:
	rm -f *.o
clobber: clean
	rm -f libg.a
