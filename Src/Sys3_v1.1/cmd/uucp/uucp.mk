#	/*  uucp.mk 3.18  4/4/80  */
#	/*  11/45, 11/70, and VAX version ('-i' has no effect on VAX)	*/
#	/* for 11/23, 11/34 (without separate I/D), IFLAG= */
CC=cc
CFLAGS=-O
LDFLAGS=-s
IFLAG=-i
LIBS=
FRC=
OWNER=uucp
INSDIR=/usr/lib/uucp
BIN=/usr/bin
UPATH=.
PUBDIR=/usr/spool/uucppublic
SPOOL=/usr/spool/uucp
XQTDIR=/usr/lib/uucp/.XQTDIR
OLDLOG=/usr/lib/uucp/.OLD
PKON=pkon.o
IOCTL=
CLEAN=
LINTOP=-hbau
COMMANDS=uucp uux uuxqt uucico uulog uuclean uuname uustat uusub
OFILES=assert.o cpmv.o expfile.o gename.o getpwinfo.o index.o lastpart.o \
	prefix.o shio.o ulockf.o xqt.o
LFILES=assert.c cpmv.c expfile.c gename.c getpwinfo.c index.c lastpart.c \
	prefix.c shio.c ulockf.c xqt.c
OUUCP=uucpdefs.o uucp.o gwd.o chkpth.o getargs.o logent.o uucpname.o\
	versys.o us_crs.o us_open.o
LUUCP=uucpdefs.c uucp.c gwd.c chkpth.c getargs.c logent.c uucpname.c\
	versys.c us_crs.c us_open.c
OUUX=uucpdefs.o uux.o gwd.o anyread.o chkpth.o getargs.o getprm.o\
	logent.o uucpname.o versys.o
LUUX=uucpdefs.c uux.c gwd.c anyread.c chkpth.c getargs.c getprm.c\
	logent.c uucpname.c versys.c
OUUXQT=uucpdefs.o uuxqt.o mailst.o getprm.o gnamef.o logent.o uucpname.o \
	chkpth.o getargs.o anyread.o
LUUXQT=uucpdefs.c uuxqt.c mailst.c getprm.c gnamef.c logent.c uucpname.c \
	chkpth.c getargs.c anyread.c
OUUCICO=uucpdefs.o cico.o conn.o cntrl.o pk0.o pk1.o gio.o anyread.o \
	anlwrk.o chkpth.o getargs.o gnamef.o gnsys.o gnxseq.o \
	imsg.o logent.o sysacct.o systat.o \
	mailst.o uucpname.o us_rrs.o us_sst.o us_open.o ub_sst.o setline.o
LUUCICO=uucpdefs.c cico.c conn.c cntrl.c pk0.c pk1.c gio.c anyread.c \
	anlwrk.c chkpth.c getargs.c gnamef.c gnsys.c gnxseq.c \
	imsg.c logent.c sysacct.c systat.c \
	mailst.c uucpname.c us_rrs.c us_sst.c us_open.c ub_sst.c setline.c
OUULOG=uucpdefs.o uulog.o prefix.o xqt.o ulockf.o gnamef.o assert.o
LUULOG=uucpdefs.c uulog.c prefix.c xqt.c ulockf.c gnamef.c assert.c
OUUCLEAN=uucpdefs.o uuclean.o gnamef.o prefix.o mailst.o getpwinfo.o\
	 getargs.o
LUUCLEAN=uucpdefs.c uuclean.c gnamef.c prefix.c mailst.c getpwinfo.c\
	 getargs.c
OUUNAME=uuname.o uucpname.o uucpdefs.o
LUUNAME=uuname.c uucpname.c uucpdefs.c
OUUSTAT=uucpdefs.o uustat.o gnamef.o getpwinfo.o us_open.o getopt.o \
	lastpart.o cpmv.o
LUUSTAT=uucpdefs.c uustat.c gnamef.c getpwinfo.c us_open.c getopt.c \
	lastpart.c cpmv.c
OUUSUB=uucpdefs.o uusub.o getpwinfo.o us_open.o xqt.o getopt.o
LUUSUB=uucpdefs.c uusub.c getpwinfo.c us_open.c xqt.c getopt.c
INIT=init

all:	$(INIT) $(COMMANDS) 

install:	all cp new $(CLEAN)

new:		mkdirs cpfiles

cp:	all
	chown $(OWNER) $(INSDIR)
	chmod 755 $(INSDIR)
	cp uucp $(BIN)
	chmod 4111 $(BIN)/uucp
	chown $(OWNER) $(BIN)/uucp
	cp uux $(BIN)
	chmod 4111 $(BIN)/uux
	chown $(OWNER) $(BIN)/uux
	cp uuxqt $(INSDIR)
	chmod 4111 $(INSDIR)/uuxqt
	chown $(OWNER) $(INSDIR)/uuxqt
	cp uucico $(INSDIR)
	chmod 4111 $(INSDIR)/uucico
	chown $(OWNER) $(INSDIR)/uucico
	cp uulog $(BIN)
	chmod 4111 $(BIN)/uulog
	chown $(OWNER) $(BIN)/uulog
	cp uuclean $(INSDIR)
	chmod 4111 $(INSDIR)/uuclean
	chown $(OWNER) $(INSDIR)/uuclean
	cp uuname $(BIN)
	chmod 4111 $(BIN)/uuname
	chown $(OWNER) $(BIN)/uuname
	cp uustat $(BIN)
	chmod 4111 $(BIN)/uustat
	chown $(OWNER) $(BIN)/uustat
	cp uusub $(INSDIR)
	chmod 100 $(INSDIR)/uusub
	chown $(OWNER) $(INSDIR)/uusub

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(COMMANDS)

get:
	cp $(UPATH)/* .

cmp:	all
	cmp uucp $(BIN)
	rm uucp
	cmp uux $(BIN)
	rm uux
	cmp uuxqt $(INSDIR)/uuxqt
	rm uuxqt
	cmp uucico $(INSDIR)/uucico
	rm uucico
	cmp uulog $(BIN)
	rm uulog
	cmp uuclean $(INSDIR)/uuclean
	rm uuclean
	cmp uuname $(BIN)
	rm uuname
	cmp uustat $(BIN)
	rm uustat
	cmp uusub $(INSDIR)
	rm uusub
	rm *.o


init:	anlwrk.o anyread.o chkpth.o cpmv.o expfile.o gename.o \
	getargs.o getprm.o getpwinfo.o gnamef.o gnsys.o \
	gnxseq.o gwd.o imsg.o index.o lastpart.o logent.o \
	prefix.o mailst.o shio.o sysacct.o \
	getopt.o \
	systat.o ulockf.o uucpname.o versys.o xqt.o

uucp:	$(OUUCP) $(OFILES)
	$(CC) $(LDFLAGS) $(OUUCP) $(OFILES) $(LIBS) -o uucp

uux:	$(OUUX) $(OFILES)
	$(CC) $(LDFLAGS) $(OUUX) $(OFILES) $(LIBS) -o uux

uuxqt:	$(OUUXQT) $(OFILES)
	$(CC) $(LDFLAGS) $(OUUXQT) $(OFILES) $(LIBS) -o uuxqt

uucico:	$(OUUCICO) $(OFILES) $(IOCTL) $(PKON)
	$(CC) $(IFLAG) $(LDFLAGS) $(OUUCICO) $(OFILES) $(IOCTL) $(PKON) $(LIBS)\
	-o uucico

uulog:	$(OUULOG)
	$(CC) $(LDFLAGS) $(OUULOG) $(LIBS) -o uulog

uuclean:  $(OUUCLEAN)
	$(CC) $(LDFLAGS) $(OUUCLEAN) $(LIBS) -o uuclean

uuname:	$(OUUNAME)
	$(CC) $(LDFLAGS) $(OUUNAME) $(LIBS) -o uuname
 
uustat:	$(OUUSTAT)
	$(CC) $(LDFLAGS) $(OUUSTAT) $(LIBS) -o uustat

uusub:	$(OUUSUB)
	$(CC) $(LDFLAGS) $(OUUSUB) $(LIBS) -o uusub


ub_sst.o uusub.o:	uusub.h

cico.o:	uusub.h uust.h uucp.h

anlwrk.o cntrl.o us_crs.o us_open.o us_rrs.o\
	us_sst.o uucp.o uustat.o:	uust.h uucp.h

anyread.o assert.o chkpth.o cico.o conn.o cpmv.o expfile.o gename.o\
	getpwinfo.o gio.o gnamef.o gnsys.o gnxseq.o gwd.o imsg.o ioctl.o\
	logent.o mailst.o sdmail.o setline.o shio.o\
	systat.o ulockf.o uuclean.o uucpdefs.o uucpname.o uulog.o uuname.o\
	uux.o uuxqt.o versys.o xqt.o:	uucp.h

FRC:

mkdirs:
	-mkdir $(INSDIR)
	chmod 755 $(INSDIR)
	-mkdir $(SPOOL)
	chmod 777 $(SPOOL)
	chown $(OWNER) $(SPOOL)
	-mkdir $(PUBDIR)
	chmod 777 $(PUBDIR)
	chown $(OWNER) $(PUBDIR)
	-mkdir $(XQTDIR)
	chmod 777 $(XQTDIR)
	chown $(OWNER) $(XQTDIR)
	-mkdir $(OLDLOG)
	chmod 777 $(OLDLOG)
	chown $(OWNER) $(OLDLOG)

cpfiles:
	-cp $(UPATH)/L* $(UPATH)/USERFILE $(INSDIR)
	-cp $(UPATH)/uudemon* $(INSDIR)
	-chmod 755 $(INSDIR)/uudemon*
	-chmod 400 $(INSDIR)/L.sys $(INSDIR)/USERFILE
	-chmod 444 $(INSDIR)/L-*
	-chown $(OWNER) $(INSDIR)/*

#  lint procedures

lint:	lintuucp lintuucico lintuux lintuuxqt lintuulog lintuuclean\
	lintuuname lintuustat lintuusub
lintuucp:
	lint $(LINTOP) $(LUUCP) $(LFILES)

lintuucico:
	lint $(LINTOP) $(LUUCICO) $(LFILES)

lintuux:
	lint $(LINTOP) $(LUUX) $(LFILES)

lintuuxqt:
	lint $(LINTOP) $(LUUXQT) $(LFILES)

lintuulog:
	lint $(LINTOP) $(LUULOG)

lintuuclean:
	lint $(LINTOP) $(LUUCLEAN)

lintuuname:
	lint $(LINTOP) $(LUUNAME)

lintuustat:
	lint $(LINTOP) $(LUUSTAT) $(LFILES)

lintuusub:
	lint $(LINTOP) $(LUUSUB) $(LFILES)

