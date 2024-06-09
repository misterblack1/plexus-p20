CFLAGS = -O
LDFLAGS = -s -n
LIBES =
INS = :
FRC =
TESTDIR = .
COMDIR = /usr/bin
DEMDIR = /usr/lib

all:	lpr vpr lpd vpd vpd.pr
spoolers:	lpr tpr vpr
daemons:	lpd tpd tpd.pr vpd vpd.pr

lpr::	lpr.c spool.c $(FRC)
	$(CC) -DNMAIL $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/lpr lpr.c $(LIBES)
lpr::
	$(INS) -n $(COMDIR) $(TESTDIR)/lpr

tpr::	lpr.c spool.c $(FRC)
	$(CC) -DSPDIR='"/usr/spool/tpd"' -DDAEMON='"/usr/lib/tpd"' -DNMAIL\
		$(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/tpr lpr.c $(LIBES)
tpr::
	$(INS) -n $(COMDIR) $(TESTDIR)/tpr

vpr::	lpr.c spool.c $(FRC)
	$(CC) -DSPDIR='"/usr/spool/vpd"' -DDAEMON='"/usr/lib/vpd"' -DNMAIL\
		-DVPR $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/vpr lpr.c $(LIBES)
vpr::
	$(INS) -n $(COMDIR) $(TESTDIR)/vpr

lpd::	lpd.o banner.o $(FRC)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/lpd lpd.o banner.o $(LIBES)
lpd::
	$(INS) -n $(DEMDIR) $(TESTDIR)/lpd
lpd.o:	lpd.c daemon.c daemon0.c $(FRC)

tpd::	vlpd.c daemon.c daemon0.c $(FRC)
	$(CC) -DSPDIR='"/usr/spool/tpd"' -DLOCK='"/usr/spool/tpd/lock"'\
		-DPRT='"/usr/lib/tpd.pr"'\
		$(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/tpd vlpd.c $(LIBES)
tpd::
	$(INS) -n $(DEMDIR) $(TESTDIR)/tpd

tpd.pr::	prt1200.o banner.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/tpd.pr prt1200.o banner.o
tpd.pr::
	$(INS) -n $(DEMDIR) $(TESTDIR)/tpd.pr

vpd::	vlpd.c daemon.c daemon0.c $(FRC)
	$(CC) -DSPDIR='"/usr/spool/vpd"' -DLOCK='"/usr/spool/vpd/lock"'\
		-DPRT='"/usr/lib/vpd.pr"'\
		$(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/vpd vlpd.c $(LIBES)
vpd::
	$(INS) -n $(DEMDIR) $(TESTDIR)/vpd

vpd.pr::	vpd.pr.o banner.o $(FRC)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/vpd.pr vpd.pr.o banner.o $(LIBES)
vpd.pr::
	$(INS) -n $(DEMDIR) $(TESTDIR)/vpd.pr

install:
	make -f lpr.mk all INS=/etc/install FRC=$(FRC)\
		COMDIR=$(COMDIR) DEMDIR=$(DEMDIR) TESTDIR=$(TESTDIR)\
		"CFLAGS=$(CFLAGS)" "LDFLAGS=$(LDFLAGS)" "LIBES=$(LIBES)" 

clean:
	rm -f *.o

clobber:	clean
	cd $(TESTDIR);\
	rm -f lpr tpr vpr
	rm -f lpd tpd tpd.pr vpd vpd.pr

FRC:
