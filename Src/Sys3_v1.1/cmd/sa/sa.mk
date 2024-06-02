# how to use this makefile
# to make sure all files  are up to date: make -f sa.mk all
#
# to force recompilation of all files: make -f sa.mk all FRC=FRC 
#
# to test new executables before installing in 
# /usr/lib/sa:	make -f sa.mk testbi
#
# to install just one file:	make -f sa.mk safile "INS=/etc/install"
#
TESTDIR = .
FRC =
INS = :
INSDIR = /usr/lib/sa
CFLAGS = -O 
LDFLAGS = -s
FFLAG =
 

all:	sadd sadc sarpt sa1 sar timex sag


sadd:: sadd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/sadd sadd.c
sadd::
	$(INS) -n $(INSDIR) $(TESTDIR)/sadd $(INSDIR)
sadc:: sadc.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/sadc sadc.c 
sadc::
	$(INS) -o -n $(INSDIR) $(TESTDIR)/sadc $(INSDIR)
sarpt:: sarpt.c
	$(CC) $(FFLAG) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/sarpt sarpt.c
sarpt::
	$(INS) -n $(INSDIR) $(TESTDIR)/sarpt $(INSDIR)
sar:: sar.sh
	cp sar.sh sar
sar::
	$(INS) -n $(INSDIR) $(TESTDIR)/sar $(INSDIR)
sa1:: sa1.sh
	cp sa1.sh sa1
sa1::
	$(INS) -n $(INSDIR) $(TESTDIR)/sa1 $(INSDIR)
 
timex::	timex.c 
	$(CC) $(FFLAG) $(CFLAGS)  -o $(TESTDIR)/timex timex.c 
timex::
	$(INS) -n /usr/bin $(TESTDIR)/timex
sag::	sag.c
	$(CC) $(FFLAG) $(CFLAGS) -o $(TESTDIR)/sag sag.c 
sag::
	$(INS) -n /usr/bin $(TESTDIR)/sag
test:		testai

testbi:		#test for before installing
	sh  $(TESTDIR)/runtest new /usr/src/cmd/sa

testai:		#test for after install
	sh $(TESTDIR)/runtest new

install:
	make -f sa.mk all FFLAG=$(FFLAG) "INS=/etc/install" INSDIR=$(INSDIR) \
		LDFLAGS="$(LDFLAGS)"

clean:
	-rm -f *.o
 
clobber:	clean
		-rm -f sar sadd sadc sarpt sa1 sag timex

FRC:
