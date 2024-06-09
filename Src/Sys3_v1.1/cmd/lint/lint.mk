MIP = mip
LINT = lint
LINT1 = ./lint1
INS = /etc/install
TMPDIR = /usr/tmp
LFLAGS = -i
CFLAGS = -O -DLINT -I$(MIP) -I. -DBUG4
FRC =
MFILES = macdefs $(MIP)/manifest $(MIP)/mfile1

#object files for the first and second passes
OFILES1 = trees.o pftn.o scan.o comm1.o messages.o optim.o xdefs.o cgram.o\
	lerror.o msgbuf.o lint.o
OFILES2 = lpass2.o lerror2.o messages.o msgbuf2.o

all:	lint1 lint2 llib-lc.ln llib-port.ln 

#makes for the first pass of lint
lint1:	$(OFILES1) $(FRC)
	$(CC) $(LFLAGS) -o lint1 $(OFILES1)

trees.o:	$(MIP)/messages.h $(MFILES) $(MIP)/trees.c
			$(CC) $(CFLAGS) -c  $(MIP)/trees.c
pftn.o:		$(MIP)/messages.h $(MFILES) $(MIP)/pftn.c
			$(CC) $(CFLAGS) -c  $(MIP)/pftn.c
scan.o:		$(MIP)/messages.h $(MFILES) $(MIP)/scan.c
			$(CC) $(CFLAGS) -c  $(MIP)/scan.c
comm1.o:	$(MIP)/common $(MFILES)     $(MIP)/comm1.c
			$(CC) $(CFLAGS) -c  $(MIP)/comm1.c
messages.o:	$(MIP)/messages.h           $(MIP)/messages.c
			$(CC) $(CFLAGS) -c  $(MIP)/messages.c
optim.o:	$(MFILES)                   $(MIP)/optim.c
			$(CC) $(CFLAGS) -c  $(MIP)/optim.c
xdefs.o:	$(MFILES)                   $(MIP)/xdefs.c
			$(CC) $(CFLAGS) -c  $(MIP)/xdefs.c
cgram.o:	$(MIP)/messages.h $(MFILES) 
cgram.c:	$(MIP)/cgram.y
		yacc $(MIP)/cgram.y ; mv y.tab.c cgram.c
lerror.o:	lerror.h $(MIP)/messages.h 
msgbuf.o:	lerror.h $(MIP)/messages.h
lint.o:		lerror.h lmanifest $(MIP)/messages.h $(MFILES)

#makes for the lint libraries
llib-lc.ln:  llib-lc $(LINT1) $(FRC)
	/lib/cpp -C llib-lc | $(LINT1) -vx -H$(TMPDIR)/hlint >llib-lc.ln 
	rm $(TMPDIR)/hlint
llib-port.ln:  llib-port $(LINT1) $(FRC)
	/lib/cpp -C llib-port | $(LINT1) -vxp -H$(TMPDIR)/hlint >llib-port.ln 
	rm $(TMPDIR)/hlint 

#makes for the second pass of lint
lint2:	$(OFILES2) $(FRC)
	$(CC) $(LFLAGS) -o lint2 $(OFILES2)
msgbuf2.o:	lerror.h 
lpass2.o:	lerror.h lmanifest lpass2.h $(MIP)/manifest
lerror2.o:	lerror.h lmanifest lpass2.h $(MIP)/manifest $(MIP)/messages.h

#miscellaneous utilities

# to reinstall libraries after clobber has been used, try :
#	make inslibs LINT1=/usr/lib/lint1
#  (this uses the installed version of lint pass 1)
inslibs: llib-lc.ln llib-port.ln
	$(INS) -n /usr/lib llib-lc
	$(INS) -n /usr/lib llib-lc.ln
	$(INS) -n /usr/lib llib-port
	$(INS) -n /usr/lib llib-port.ln
install: all inslibs
	$(INS) -n /usr/lib lint1
	$(INS) -n /usr/lib lint2
	cp lint.sh lint
	chmod 0755 lint
	$(INS) -n /usr/bin lint

lintall:
	$(LINT) -DLINT -pvb -I. -I$(MIP)  cgram.c $(MIP)/xdefs.c $(MIP)/scan.c \
	$(MIP)/pftn.c $(MIP)/trees.c $(MIP)/optim.c lint.c $(MIP)/messages.c \
	msgbuf.c lerror.c
	$(LINT) -DLINT -hp -I$(MIP) -I. lpass2.c lerror2.c msgbuf2.c \
	$(MIP)/messages.c
clean:
	rm -f *.o cgram.c
clobber: clean
	rm -f lint1 lint2 llib-lc.ln llib-port.ln lint
FRC:
