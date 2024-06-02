INS = :
LN = :
INSDIR =
TESTDIR = .

SRC=.
FRC=
CFLAGS=-O
LDFLAGS=-s -n

all:	cref crpost upost atab ctab etab aign cign eign

cref::	dr.o put.o acts.o $(FRC)
	$(CC) $(LDFLAGS) dr.o put.o acts.o -o $(TESTDIR)/cref
cref::
	$(INS) -n /usr/bin $(TESTDIR)/cref

dr.o: ccmn.h mcons.h
acts.o: ccmn.h mcons.h

crpost::	crpost.c $(FRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/crpost crpost.c
crpost::
	$(INS) -f /usr/lib/cref $(TESTDIR)/crpost

upost::	upost.c $(FRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/upost upost.c
upost::
	$(INS) -f /usr/lib/cref $(TESTDIR)/upost
mtab: mtab.c
	$(CC) mtab.c -o mtab
atab:: mtab atable
	./mtab atable $(TESTDIR)/atab
ctab:: mtab ctable
	./mtab ctable $(TESTDIR)/ctab
etab:: mtab etable
	./mtab etable $(TESTDIR)/etab
atab::
	$(INS) -f /usr/lib/cref $(TESTDIR)/atab 
ctab::
	$(INS) -f /usr/lib/cref $(TESTDIR)/ctab 
etab::
	$(INS) -f /usr/lib/cref $(TESTDIR)/etab 
mign: mign.c
	$(CC) mign.c -o mign
aign:: mign asym
	./mign asym $(TESTDIR)/aign
cign:: mign csym
	./mign csym $(TESTDIR)/cign
eign:: esym
	cp esym $(TESTDIR)/eign
aign::
	$(INS) -f /usr/lib/cref $(TESTDIR)/aign 
cign::
	$(INS) -f /usr/lib/cref $(TESTDIR)/cign 
eign::
	$(INS) -f /usr/lib/cref $(TESTDIR)/eign 
	@set +e; if test $(LN) = ln ; then rm -f /usr/lib/eign ; fi
	$(LN) /usr/lib/cref/eign /usr/lib/eign

install:  all
	make -f cref.mk all LDFLAGS="$(LDFLAGS)" INS=/etc/install LN=ln \
		INSDIR=$(INSDIR) TESTDIR=$(TESTDIR)

clean:
	rm -f *.o
	rm -f mtab mign

clobber: clean
	cd $(TESTDIR); rm -f cref crpost upost *ign *tab
	rm -f *ign *tab

FRC:
