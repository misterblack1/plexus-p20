#	@(#)makefile	2.1	SCCS id keyword
#
# Skeleton for version 7
#
#BINDIR=	/usr/plx/bin
BINDIR=	/usr/plx
NBINDIR=/usr/plx/new/bin
CFLAGS=	-O -DTELL -DBUFSIZ=1024 -DSYS3
XSTR=	xstr
ED=	-ed
AS=	-as -u
RM=	-rm
CTAGS=	ctags
CXREF=	cxref

#
# strings.o must be last in OBJS since it can change when previous files compile
#
OBJS=sh.o sh.dol.o sh.err.o sh.exec.o sh.exp.o sh.func.o sh.glob.o sh.hist.o \
	sh.lex.o sh.misc.o sh.parse.o sh.print.o sh.sem.o sh.set.o \
	sh.wait.o alloc.o sh.init.o pprintf.o dup2.o ${PW} \
	strings.o

#
# Special massaging of C files for sharing of strings
#
.c.o:
	${CC} -E ${CFLAGS} $*.c | ${XSTR} -c -
	${CC} -c ${CFLAGS} x.c 
	mv x.o $*.o

# We are running csh split I/D because of a bug (probably in the system)
# where it only works right split I/D if you create a 1 line file as
#	set date = `date`
# and source the file.  If this works OK for you you can probably run it
# shared but not I/D.  It should thrash less if you do.
csh:	${OBJS} sh.local.h
#	${CC} -n -o csh ${OBJS}
	${CC} -s -i -o csh ${OBJS}
#	echo text must be less than 40960
	size csh

#
# strings.o and sh.init.o are specially processed to be shared
# Don't undo this lightly.  You must undo one if you exceed 40K,
# and must undo both to run split I/D.  But if you do this you
# get grossly larger unsharable data segments.
#
strings.o: strings
	${XSTR}
	${CC} -S xs.c
# to undo comment out next line.
#	${ED} - <:rofix xs.s
	${AS} -o strings.o xs.s
	${RM} xs.s

sh.init.o:
	${CC} -E ${CFLAGS} sh.init.c | ${XSTR} -c -
	${CC} ${CFLAGS} -S x.c
# to undo comment out next line.
#	${ED} - <:rofix x.s
	${AS} -o sh.init.o x.s
	${RM} x.s
	
lint:
	lint sh*.h sh*.c

print:
	@pr READ_ME
	@pr makefile makefile.*
	@(size -l csh; size *.o) | pr -h SIZES
	@${CXREF} sh*.c | pr -h XREF
	@ls -l | pr 
	@pr sh*.h [a-rt-z]*.h sh*.c alloc.c

install: csh sh.local.h
	chown bin csh
	chgrp bin csh
	chmod 755 csh
	install -o -f /usr/plx csh
	cp .login /usr/plx/login
	cp .cshrc /usr/plx/cshrc
	cp /etc/ttytype /usr/plx
	cp cshprofile.sh /etc/cshprofile
	chown bin /etc/cshprofile
	chgrp bin /etc/cshprofile
	chmod 664 /etc/cshprofile
#	mv ${BINDIR}/csh ${BINDIR}/ocsh
#	${RM} -f ${BINDIR}/ocsh
#	cp csh ${BINDIR}/csh

#ninstall: csh sh.local.h
#	mv ${NBINDIR}/csh ${NBINDIR}/oncsh
#	cp csh ${NBINDIR}/csh
#	chown bin ${NBINDIR}/csh
#	chmod 755 ${NBINDIR}/csh

clean:
	${RM} -f strings errs x.c xs.c
	${RM} -f *.o

clobber:
	${RM} -f strings errs x.c xs.c
	${RM} -f *.o
	${RM} -f csh

tags:
	${CTAGS} sh*.c

sh.o:	sh.c sh.h sh.local.h
