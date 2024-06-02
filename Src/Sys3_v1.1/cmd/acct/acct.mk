TESTDIR = .
FRC =
INCLUDE = /usr/include
BIN = /bin
UBIN = /usr/bin
INS = /etc/install
ARGS =
INSDIR = /usr/lib/acct
CFLAGS = -O 
LDFLAGS = -s
LLIB = lib/a.a $(LIB)

all:	library acctcms acctcom acctcon1\
	acctcon2 acctdisk acctdusg acctmerg accton\
	acctprc1 acctprc2 acctwtmp\
	fwtmp wtmpfix\
	chargefee ckpacct dodisk holidays lastlogin\
	monacct nulladm prctmp prdaily\
	prtacct remove runacct\
	sdisk shutacct startup turnacct

library:
	cd lib;  make "INS=$(INS)" "INCLUDE=$(INCLUDE)"

acctcms:	lib/a.a acctcms.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctcms.c $(LLIB) -o $(TESTDIR)/acctcms
		$(INS) -f $(INSDIR) $(TESTDIR)/acctcms

acctcom:	lib/a.a acctcom.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctcom.c $(LLIB) -o $(TESTDIR)/acctcom
		$(INS) -f /bin $(TESTDIR)/acctcom

acctcon1:	lib/a.a acctcon1.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctcon1.c  $(LLIB) -o $(TESTDIR)/acctcon1
		$(INS) -f $(INSDIR) $(TESTDIR)/acctcon1

acctcon2:	lib/a.a acctcon2.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctcon2.c $(LLIB) -o $(TESTDIR)/acctcon2
		$(INS) -f $(INSDIR) $(TESTDIR)/acctcon2
 
acctdisk:	lib/a.a acctdisk.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctdisk.c $(LLIB) -o $(TESTDIR)/acctdisk
		$(INS) -f $(INSDIR) $(TESTDIR)/acctdisk

acctdusg:	lib/a.a acctdusg.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctdusg.c $(LLIB) -o $(TESTDIR)/acctdusg
		$(INS) -f $(INSDIR) $(TESTDIR)/acctdusg

acctmerg:	lib/a.a acctmerg.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctmerg.c $(LLIB) -o $(TESTDIR)/acctmerg
		$(INS) -f $(INSDIR) $(TESTDIR)/acctmerg

accton:	lib/a.a accton.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		accton.c $(LLIB) -o $(TESTDIR)/accton
		$(INS) -f $(INSDIR) $(TESTDIR)/accton
		chown root $(INSDIR)/accton
		chmod 4755 $(INSDIR)/accton

acctprc1:	lib/a.a acctprc1.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctprc1.c $(LLIB) -o $(TESTDIR)/acctprc1
		$(INS) -f $(INSDIR) $(TESTDIR)/acctprc1
 
acctprc2:	lib/a.a acctprc2.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctprc2.c $(LLIB) -o $(TESTDIR)/acctprc2
		$(INS) -f $(INSDIR) $(TESTDIR)/acctprc2

acctwtmp:	lib/a.a acctwtmp.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		acctwtmp.c $(LLIB) -o $(TESTDIR)/acctwtmp
		$(INS) -f $(INSDIR) $(TESTDIR)/acctwtmp

fwtmp:		lib/a.a fwtmp.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		fwtmp.c $(LLIB) -o $(TESTDIR)/fwtmp
		$(INS) -f $(INSDIR) $(TESTDIR)/fwtmp

wtmpfix:	lib/a.a wtmpfix.c $(FRC)
		$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS)\
		wtmpfix.c $(LLIB) -o $(TESTDIR)/wtmpfix
		$(INS) -f $(INSDIR) $(TESTDIR)/wtmpfix

chargefee:	chargefee.sh $(FRC)
		cp chargefee.sh $(TESTDIR)/chargefee
		$(INS) -f $(INSDIR) $(TESTDIR)/chargefee

ckpacct:	ckpacct.sh $(FRC)
		cp ckpacct.sh $(TESTDIR)/ckpacct
		$(INS) -f $(INSDIR) $(TESTDIR)/ckpacct

dodisk:		dodisk.sh $(FRC)
		cp dodisk.sh $(TESTDIR)/dodisk
		$(INS) -f $(INSDIR) $(TESTDIR)/dodisk

monacct:	monacct.sh $(FRC)
		cp monacct.sh $(TESTDIR)/monacct
		$(INS) -f $(INSDIR) $(TESTDIR)/monacct


holidays:	holidays.sh $(FRC)
		cp holidays.sh $(TESTDIR)/holidays
		$(INS) -f $(INSDIR) $(TESTDIR)/holidays
 
lastlogin:	lastlogin.sh $(FRC)
		cp lastlogin.sh $(TESTDIR)/lastlogin
		$(INS) -f $(INSDIR) $(TESTDIR)/lastlogin
 
nulladm:	nulladm.sh $(FRC)
		cp nulladm.sh $(TESTDIR)/nulladm
		$(INS) -f $(INSDIR) $(TESTDIR)/nulladm
 
prctmp:		prctmp.sh $(FRC)
		cp prctmp.sh $(TESTDIR)/prctmp
		$(INS) -f $(INSDIR) $(TESTDIR)/prctmp

prdaily:	prdaily.sh $(FRC)
		cp prdaily.sh $(TESTDIR)/prdaily
		$(INS) -f $(INSDIR) $(TESTDIR)/prdaily
 
prtacct:	prtacct.sh $(FRC)
		cp prtacct.sh $(TESTDIR)/prtacct
		$(INS) -f $(INSDIR) $(TESTDIR)/prtacct
 
remove:		remove.sh $(FRC)
		cp remove.sh $(TESTDIR)/remove
		$(INS) -f $(INSDIR) $(TESTDIR)/remove
 
runacct:	runacct.sh $(FRC)
		cp runacct.sh $(TESTDIR)/runacct
		$(INS) -f $(INSDIR) $(TESTDIR)/runacct
 
sdisk:		sdisk.sh $(FRC)
		cp sdisk.sh $(TESTDIR)/sdisk
		$(INS) -f $(INSDIR) $(TESTDIR)/sdisk

shutacct:	shutacct.sh $(FRC)
		cp shutacct.sh $(TESTDIR)/shutacct
		$(INS) -f $(INSDIR) $(TESTDIR)/shutacct

startup:	startup.sh $(FRC)
		cp startup.sh $(TESTDIR)/startup
		$(INS) -f $(INSDIR) $(TESTDIR)/startup

turnacct:	turnacct.sh $(FRC)
		cp turnacct.sh $(TESTDIR)/turnacct
		$(INS) -f $(INSDIR) $(TESTDIR)/turnacct
 
install:
	make -f acct.mk $(ARGS) "INCLUDE=$(INCLUDE)" "LIB=$(LIB)" \
		"LDFLAGS=$(LDFLAGS)" "CFLAGS=$(CFLAGS)"

clean:
	-rm -f *.o
	cd lib; make clean
 
clobber:	clean
		-rm -f acctcms acctcom acctcon1 acctcon2 acctdisk\
		acctdusg acctmerg accton acctprc1 acctprc2 acctwtmp\
		fwtmp wtmpfix
		-rm -f chargefee ckpacct dodisk lastlogin nulladm\
		monacct prctmp prdaily prtacct remove runacct\
		sdisk shutacct startup turnacct holidays
		cd lib; make clobber

FRC:
