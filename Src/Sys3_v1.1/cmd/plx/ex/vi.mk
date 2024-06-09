VERSION=2.13
# @(#)makefile	13.3 9/23/80
#
# Ex skeletal makefile for Unix 3.0
#
# NB: This makefile doesn't indicate any dependencies on header files.
#
# Ex is very large - it may not fit on PDP-11's depending on the operating
# system and the cflags you turn on. Things that can be turned off to save
# space include LISPCODE (-l flag, showmatch and lisp options), UCVISUAL
# (visual \ nonsense on upper case only terminals), CHDIR (the undocumented
# chdir command,)  RDONLY (-R flag, view), SOFTTABS (^T tabs), ZCMD (the
# various strange options after z plus ^F/^B), ZZCMD (ZZ from visual), WRAPMGN
# (wrapmargin), TAGSCODE (the tags feature), OPENCODE (open without visual),
# CRNL (code to support terminals with cr, nl, or ns attributes),
# BEEHIVE (superbee: f1=esc, f2=^c)
# Rule of thumb: OPENCODE will buy you about 2.5K, LISPCODE and TAGSCODE
# are worth perhaps 1.5K, and the others are worth fairly small amounts
# (mostly about 50-100 bytes).  UCVISUAL is worth a bit more (256?) and
# you should take it out unless you have such a terminal.
# Also make sure you are running the doprnt printf and not the portable
# one unless you have to run the portable one.
#
# Don't define VFORK unless your system has the VFORK system call,
# which is like fork but the two processes share memory until the
# child execs. This speeds up ex by saving the memory copy.
#
# If your system expands tabs to 4 spaces you should -DTABS=4 below
#
BINDIR=	/usr/plx
INSDIR=	/usr/plx
LIBDIR=	/usr/lib

FOLD=	${BINDIR}/fold
CTAGS=	${BINDIR}/ctags
CXREF=	${BINDIR}/cxref
#
#		 **** REQUIRED TO MAKE VI *********
#
XSTR=	${BINDIR}/xstr
MKSTR=	${BINDIR}/mkstr
TERMLIB=/usr/lib/libtermlib.a

TINCLUDE=./
PR=	pr
# Debugging must be done on a vax, otherwise it just won't fit.
DEB=	${NONDEBUGFLAGS}	# or ${DEBUGFLAGS} if mdd
DEBUGFLAGS=	-DTRACE -g
NONDEBUGFLAGS=	-O
CFLAGS=	${OPTIONS} ${DEB} -I${TINCLUDE} -DUSG3TTY
# See also CFLAGS in the termlib makefile.
OPTIONS= -DZCMD -DZZCMD -DRDONLY -DWRAPMGN 
# everything:	-DLISPCODE -DCHDIR -DUCVISUAL -DSOFTTABS -DZCMD -DZZCMD -DRDONLY -DWRAPMGN -DTAGSCODE -DOPENCODE -DCRNL -DBEEHIVE
# PRINTF can be "pprintf.o" for the portable version or "printf.o doprnt.o"
# for the nonportable version, which uses doprnt (written in pdp-11 assembly
# language) and is quite a bit smaller than pprintf.o (about 900 bytes)
PRINTF=	pprintf.o
# PRINTF=printf.o doprnt.o
OBJS=	ex.o ex_addr.o ex_cmds.o ex_cmds2.o ex_cmdsub.o ex_data.o ex_get.o \
	ex_io.o ex_put.o ex_re.o ex_set.o ex_subr.o ex_temp.o ex_tty.o \
	ex_unix.o ex_v.o ex_vadj.o ex_vget.o ex_vmain.o ex_voper.o \
	ex_vops.o ex_vops2.o ex_vops3.o ex_vput.o ex_vwind.o \
	${PRINTF} strings.o

.c.o:
	${MKSTR} - ex${VERSION}strings x $*.c
	${CC} -E ${CFLAGS} x$*.c | ${XSTR} -c -
	rm -f x$*.c
	${CC} ${CFLAGS} -c x.c 
	mv x.o $*.o

vi: a.out

#a.out: libtermlib.a xstr mkstr ${OBJS}
a.out: ${OBJS}
	cc -i ${OBJS} ${TERMLIB}
#	cc -i -z ${OBJS} ${TERMLIB}

#mkstr: mkstr.c
#	cc mkstr.c -o mkstr

#xstr: xstr.c
#	cc xstr.c -o xstr

# libtermlib.a :
# 	cd termlib; make install

all:	a.out exrecover expreserve tags

ex.o:	ex.c ex.h ex_argv.h ex_re.h ex_temp.h ex_tty.h ex_tune.h ex_vars.h ex_vis.h

tags:
	${CTAGS} -w *.h *.c

${OBJS}: ex_vars.h

# ex_vars.h:
# 	csh makeoptions ${CFLAGS}

strings.o: strings
	${XSTR}
	${CC} -c -S xs.c
#	ed - <:rofix xs.s
	as -o strings.o xs.s
	rm xs.s
	
exrecover: exrecover.o
	${CC} -o exrecover exrecover.o

exrecover.o:
	${CC} -c -O exrecover.c

expreserve: expreserve.o
	${CC} -o expreserve expreserve.o

expreserve.o:
	${CC} -c -O expreserve.c

clobber: 
#	If we dont have ex we cant make it so dont rm ex_vars.h
#	-rm -f expreserve exrecover
	-rm -f *.o x*.[cs]
	-rm -f a.out ex${VERSION}strings strings core trace
#	-rm -f mkstr xstr libtermlib.a
clean:
#	cd termlib; make clean
#	cd termcap; make clean
#	-mv xstr.c Xstr.c
	-rm -f *.o x*.[cs]
#	-mv Xstr.c xstr.c

install: a.out
#	cd termcap; make install
	-rm -f ${INSDIR}/ex ${INSDIR}/e /usr/bin/ex ${INSDIR}/edit \
		 ${INSDIR}/vi ${INSDIR}/view ${INSDIR}/oldex
	-cp a.out ${INSDIR}/ex
	-chown bin ${INSDIR}/ex
	-chgrp bin ${INSDIR}/ex
	-chmod 755 ${INSDIR}/ex
	-rm -f ${INSDIR}/edit ${INSDIR}/e ${INSDIR}/vi ${INSDIR}/view
	-cp ex${VERSION}strings ${LIBDIR}/ex${VERSION}strings
	-chown bin ${LIBDIR}/ex${VERSION}strings
	-chgrp bin ${LIBDIR}/ex${VERSION}strings
	-ln ${INSDIR}/ex ${INSDIR}/edit
	-chown bin ${INSDIR}/edit
	-chgrp bin ${INSDIR}/edit
	-chmod 755 ${INSDIR}/edit
	-ln ${INSDIR}/ex ${INSDIR}/e
	-chown bin ${INSDIR}/e
	-chgrp bin ${INSDIR}/e
	-chmod 755 ${INSDIR}/e
	-ln ${INSDIR}/ex ${INSDIR}/vi
	-chown bin ${INSDIR}/vi
	-chgrp bin ${INSDIR}/vi
	-chmod 755 ${INSDIR}/vi
	-ln ${INSDIR}/ex ${INSDIR}/view
	chown bin ${INSDIR}/view
	chgrp bin ${INSDIR}/view
	chmod 755 ${INSDIR}/view

installutil: exrecover expreserve
	cp exrecover ${LIBDIR}/ex${VERSION}recover
	-chown bin ${LIBDIR}/ex${VERSION}recover
	-chgrp bin ${LIBDIR}/ex${VERSION}recover
	cp expreserve ${LIBDIR}/ex${VERSION}preserve
	-chown bin ${LIBDIR}/ex${VERSION}preserve
	-chgrp bin ${LIBDIR}/ex${VERSION}preserve
#	cd ../termcap ; make install
	-mkdir /usr/preserve

lint:
	lint ex.c ex_?*.c
	lint -u exrecover.c
	lint expreserve.c

print:
	@${PR} READ* BUGS
	@${PR} makefile*
	@${PR} /etc/termcap
	@(size -l a.out ; size *.o) | ${PR} -h sizes
	@${PR} -h errno.h ${INCLUDE}/errno.h
	@${PR} -h setjmp.h ${INCLUDE}/setjmp.h
	@${PR} -h sgtty.h ${INCLUDE}/sgtty.h
	@${PR} -h signal.h ${INCLUDE}/signal.h
	@${PR} -h stat.h ${INCLUDE}/stat.h
	@${PR} -h types.h ${INCLUDE}/types.h
	@ls -ls | ${PR}
	@${CXREF} *.c | ${PR} -h XREF
	@${PR} *.h *.c

# SCCS dependencies
# ex.h: SCCS/s.ex.h; rm -f ex.h; sccs get ex.h
# ex_argv.h: SCCS/s.ex_argv.h; rm -f ex_argv.h; sccs get ex_argv.h
# ex_re.h: SCCS/s.ex_re.h; rm -f ex_re.h; sccs get ex_re.h
# ex_temp.h: SCCS/s.ex_temp.h; rm -f ex_temp.h; sccs get ex_temp.h
# ex_tty.h: SCCS/s.ex_tty.h; rm -f ex_tty.h; sccs get ex_tty.h
# ex_tune.h: SCCS/s.ex_tune.h; rm -f ex_tune.h; sccs get ex_tune.h
# ex_vars.h: SCCS/s.ex_vars.h; rm -f ex_vars.h; sccs get ex_vars.h
# ex_vis.h: SCCS/s.ex_vis.h; rm -f ex_vis.h; sccs get ex_vis.h
# ex.c: SCCS/s.ex.c; rm -f ex.c; sccs get ex.c
# ex_addr.c: SCCS/s.ex_addr.c; rm -f ex_addr.c; sccs get ex_addr.c
# ex_cmds.c: SCCS/s.ex_cmds.c; rm -f ex_cmds.c; sccs get ex_cmds.c
# ex_cmds2.c: SCCS/s.ex_cmds2.c; rm -f ex_cmds2.c; sccs get ex_cmds2.c
# ex_cmdsub.c: SCCS/s.ex_cmdsub.c; rm -f ex_cmdsub.c; sccs get ex_cmdsub.c
# ex_data.c: SCCS/s.ex_data.c; rm -f ex_data.c; sccs get ex_data.c
# ex_get.c: SCCS/s.ex_get.c; rm -f ex_get.c; sccs get ex_get.c
# ex_io.c: SCCS/s.ex_io.c; rm -f ex_io.c; sccs get ex_io.c
# ex_put.c: SCCS/s.ex_put.c; rm -f ex_put.c; sccs get ex_put.c
# ex_re.c: SCCS/s.ex_re.c; rm -f ex_re.c; sccs get ex_re.c
# ex_set.c: SCCS/s.ex_set.c; rm -f ex_set.c; sccs get ex_set.c
# ex_subr.c: SCCS/s.ex_subr.c; rm -f ex_subr.c; sccs get ex_subr.c
# ex_temp.c: SCCS/s.ex_temp.c; rm -f ex_temp.c; sccs get ex_temp.c
# ex_tty.c: SCCS/s.ex_tty.c; rm -f ex_tty.c; sccs get ex_tty.c
# ex_unix.c: SCCS/s.ex_unix.c; rm -f ex_unix.c; sccs get ex_unix.c
# ex_v.c: SCCS/s.ex_v.c; rm -f ex_v.c; sccs get ex_v.c
# ex_vadj.c: SCCS/s.ex_vadj.c; rm -f ex_vadj.c; sccs get ex_vadj.c
# ex_vget.c: SCCS/s.ex_vget.c; rm -f ex_vget.c; sccs get ex_vget.c
# ex_vmain.c: SCCS/s.ex_vmain.c; rm -f ex_vmain.c; sccs get ex_vmain.c
# ex_voper.c: SCCS/s.ex_voper.c; rm -f ex_voper.c; sccs get ex_voper.c
# ex_vops.c: SCCS/s.ex_vops.c; rm -f ex_vops.c; sccs get ex_vops.c
# ex_vops2.c: SCCS/s.ex_vops2.c; rm -f ex_vops2.c; sccs get ex_vops2.c
# ex_vops3.c: SCCS/s.ex_vops3.c; rm -f ex_vops3.c; sccs get ex_vops3.c
# ex_vput.c: SCCS/s.ex_vput.c; rm -f ex_vput.c; sccs get ex_vput.c
# ex_vwind.c: SCCS/s.ex_vwind.c; rm -f ex_vwind.c; sccs get ex_vwind.c
# printf.c: SCCS/s.printf.c; rm -f printf.c; sccs get printf.c
# doprnt.s: SCCS/s.doprnt.s; rm -f doprnt.s; sccs get doprnt.s
