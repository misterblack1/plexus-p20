#	 This is the description file used by the make(I) command
#	 to reconstruct all or part of SCCS

#	(All Macro Definitions are described on line following definition)

SRCDIR = src
#	SCCS command source directory
HDRDIR = hdr
#	Directory containing SCCS header files
TESTDIR = .
#	Current Directory
LIBDIR = lib
#	SCCS common object library directory
HELPDIR = help.d
#	Directory containing SCCS help files.
PUB_HELPLIB = /usr/lib/help
#	Public directory where help files are stored.
ARGS =
#	Arguments used when making _a_l_l or _p_a_r_t_s of SCCS with mkcmd
INSDIR = /usr/bin
#	Directory where executable SCCS commands are stored
CC = cc
#	C compiler
CFLAGS = -c -O
#	Flags used by C compiler
FFLAG =
#	Floating-point option
LDFLAGS = -s -n $(FFLAG)
#	Load flags used by C compiler
LIBES = -lPW
#	Libraries used during loading

all:		admin bdiff cdc comb \
	delta get help prs rmdel sact sccsdiff \
	unget val vc what helpfiles

install:
	make -f sccs.mk $(ARGS) LDFLAGS="$(LDFLAGS)"

admin:	admin.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o admin admin.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/admin $(INSDIR)
	-chown bin $(INSDIR)/admin
	-chgrp bin $(INSDIR)/admin
	-chmod 755 $(INSDIR)/admin

admin.o:	$(SRCDIR)/admin.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/admin.c

bdiff:	bdiff.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o bdiff bdiff.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/bdiff $(INSDIR)
	-chown bin $(INSDIR)/bdiff
	-chgrp bin $(INSDIR)/bdiff
	-chmod 755 $(INSDIR)/bdiff

bdiff.o:	$(SRCDIR)/bdiff.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/bdiff.c

cdc:	rmchg
	-rm -f $(INSDIR)/cdc
	ln $(INSDIR)/rmchg $(INSDIR)/cdc

comb:	comb.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o comb comb.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/comb $(INSDIR)
	-chown bin $(INSDIR)/comb
	-chgrp bin $(INSDIR)/comb
	-chmod 755 $(INSDIR)/comb

comb.o:		$(SRCDIR)/comb.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/comb.c

delta:	delta.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o delta delta.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/delta $(INSDIR)
	-chown bin $(INSDIR)/delta
	-chgrp bin $(INSDIR)/delta
	-chmod 755 $(INSDIR)/delta

delta.o:	$(SRCDIR)/delta.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/delta.c

get:	get.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o get get.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/get $(INSDIR)
	-chown bin $(INSDIR)/get
	-chgrp bin $(INSDIR)/get
	-chmod 755 $(INSDIR)/get

get.o:		$(SRCDIR)/get.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/get.c

help:	help.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o help help.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/help $(INSDIR)
	-chown bin $(INSDIR)/help
	-chgrp bin $(INSDIR)/help
	-chmod 755 $(INSDIR)/help

help.o:		$(SRCDIR)/help.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/help.c

prs:	prs.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o prs prs.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/prs $(INSDIR)
	-chown bin $(INSDIR)/prs
	-chgrp bin $(INSDIR)/prs
	-chmod 755 $(INSDIR)/prs

prs.o:		$(SRCDIR)/prs.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/prs.c

rmchg:	rmchg.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o rmchg rmchg.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/rmchg $(INSDIR)
	-chown bin $(INSDIR)/rmchg
	-chgrp bin $(INSDIR)/rmchg
	-chmod 755 $(INSDIR)/rmchg

rmchg.o:	$(SRCDIR)/rmchg.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/rmchg.c

rmdel:	rmchg
	-rm -f $(INSDIR)/rmdel
	ln $(INSDIR)/rmchg $(INSDIR)/rmdel

sact:	unget
	-rm -f $(INSDIR)/sact
	ln $(INSDIR)/unget $(INSDIR)/sact

sccsdiff:
	cp $(SRCDIR)/sccsdiff $(INSDIR)
	-chown bin $(INSDIR)/sccsdiff
	-chgrp bin $(INSDIR)/sccsdiff
	-chmod 755 $(INSDIR)/sccsdiff

unget:	unget.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o unget unget.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/unget $(INSDIR)
	-chown bin $(INSDIR)/unget
	-chgrp bin $(INSDIR)/unget
	-chmod 755 $(INSDIR)/unget

unget.o:	$(SRCDIR)/unget.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/unget.c

val:	val.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o val val.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/val $(INSDIR)
	-chown bin $(INSDIR)/val
	-chgrp bin $(INSDIR)/val
	-chmod 755 $(INSDIR)/val

val.o:	$(SRCDIR)/val.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/val.c

vc:	vc.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o vc vc.o $(LIBDIR)/comobj.a $(LIBES)
	cp $(TESTDIR)/vc $(INSDIR)
	-chown bin $(INSDIR)/vc
	-chgrp bin $(INSDIR)/vc
	-chmod 755 $(INSDIR)/vc

vc.o:	$(SRCDIR)/vc.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/vc.c

what:	what.o $(LIBDIR)/comobj.a
	$(CC) $(LDFLAGS) -o what what.o $(LIBDIR)/comobj.a  $(LIBES)
	cp $(TESTDIR)/what $(INSDIR)
	-chown bin $(INSDIR)/what
	-chgrp bin $(INSDIR)/what
	-chmod 755 $(INSDIR)/what

what.o:		$(SRCDIR)/what.c $(HDRDIR)/defines.h $(HDRDIR)/had.h
	$(CC) $(CFLAGS) $(SRCDIR)/what.c

$(LIBDIR)/comobj.a:
	cd lib; make -f comobj.mk comobj.a clean LDFLAGS="$(LDFLAGS)"

helpfiles:
	-mkdir $(PUB_HELPLIB)
	-chown bin $(PUB_HELPLIB)
	-chgrp bin $(PUB_HELPLIB)
	-chmod 755 $(PUB_HELPLIB)
	cp $(HELPDIR)/ad $(PUB_HELPLIB)/ad
	-chown bin $(PUB_HELPLIB)/ad
	-chgrp bin $(PUB_HELPLIB)/ad
	-chmod 644 $(PUB_HELPLIB)/ad
	cp $(HELPDIR)/bd $(PUB_HELPLIB)/bd
	-chown bin $(PUB_HELPLIB)/bd
	-chgrp bin $(PUB_HELPLIB)/bd
	-chmod 644 $(PUB_HELPLIB)/bd
	cp $(HELPDIR)/cb $(PUB_HELPLIB)/cb
	-chown bin $(PUB_HELPLIB)/cb
	-chgrp bin $(PUB_HELPLIB)/cb
	-chmod 644 $(PUB_HELPLIB)/cb
	cp $(HELPDIR)/cm $(PUB_HELPLIB)/cm
	-chown bin $(PUB_HELPLIB)/cm
	-chgrp bin $(PUB_HELPLIB)/cm
	-chmod 644 $(PUB_HELPLIB)/cm
	cp $(HELPDIR)/cmds $(PUB_HELPLIB)/cmds
	-chown bin $(PUB_HELPLIB)/cmds
	-chgrp bin $(PUB_HELPLIB)/cmds
	-chmod 644 $(PUB_HELPLIB)/cmds
	cp $(HELPDIR)/co $(PUB_HELPLIB)/co
	-chown bin $(PUB_HELPLIB)/co
	-chgrp bin $(PUB_HELPLIB)/co
	-chmod 644 $(PUB_HELPLIB)/co
	cp $(HELPDIR)/de $(PUB_HELPLIB)/de
	-chown bin $(PUB_HELPLIB)/de
	-chgrp bin $(PUB_HELPLIB)/de
	-chmod 644 $(PUB_HELPLIB)/de
	cp $(HELPDIR)/default $(PUB_HELPLIB)/default
	-chown bin $(PUB_HELPLIB)/default
	-chgrp bin $(PUB_HELPLIB)/default
	-chmod 644 $(PUB_HELPLIB)/default
	cp $(HELPDIR)/ge $(PUB_HELPLIB)/ge
	-chown bin $(PUB_HELPLIB)/ge
	-chgrp bin $(PUB_HELPLIB)/ge
	-chmod 644 $(PUB_HELPLIB)/ge
	cp $(HELPDIR)/he $(PUB_HELPLIB)/he
	-chown bin $(PUB_HELPLIB)/he
	-chgrp bin $(PUB_HELPLIB)/he
	-chmod 644 $(PUB_HELPLIB)/he
	cp $(HELPDIR)/prs $(PUB_HELPLIB)/prs
	-chown bin $(PUB_HELPLIB)/prs
	-chgrp bin $(PUB_HELPLIB)/prs
	-chmod 644 $(PUB_HELPLIB)/prs
	cp $(HELPDIR)/rc $(PUB_HELPLIB)/rc
	-chown bin $(PUB_HELPLIB)/rc
	-chgrp bin $(PUB_HELPLIB)/rc
	-chmod 644 $(PUB_HELPLIB)/rc
	cp $(HELPDIR)/un $(PUB_HELPLIB)/un
	-chown bin $(PUB_HELPLIB)/un
	-chgrp bin $(PUB_HELPLIB)/un
	-chmod 644 $(PUB_HELPLIB)/un
	cp $(HELPDIR)/ut $(PUB_HELPLIB)/ut
	-chown bin $(PUB_HELPLIB)/ut
	-chgrp bin $(PUB_HELPLIB)/ut
	-chmod 644 $(PUB_HELPLIB)/ut
	cp $(HELPDIR)/vc $(PUB_HELPLIB)/vc
	-chown bin $(PUB_HELPLIB)/vc
	-chgrp bin $(PUB_HELPLIB)/vc
	-chmod 644 $(PUB_HELPLIB)/vc

clean:		# Procedure to clean up unwanted files
	-rm -f *.o
	-rm -f  admin	bdiff	cdc	comb	delta	\
		get	help	prs	rmchg	rmdel	sact	\
		sccsdiff	unget	val	vc	what 

clobber:	clean
