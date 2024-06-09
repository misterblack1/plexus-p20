INSDIR=/usr/lib
INSTALL=/etc/install -n
M=mip
IFLAG=-i
LDFLAGS=
CFLAGS=-O
CC=cc

all:	ccom

install: all
	$(INSTALL) $(INSDIR) ccom

clean:
	rm -f *.o 

clobber: clean
	rm -f cgram.c ccom

ccom: cgram.o xdefs.o scan.o pftn.o trees.o optim.o code.o local.o reader.o\
		local2.o order.o match.o allo.o comm1.o table.o
	ld -X -o ccom $(IFLAG) $(LDFLAGS) /lib/crt0.o cgram.o xdefs.o scan.o \
		pftn.o trees.o\
		optim.o code.o local.o reader.o local2.o order.o match.o\
		allo.o comm1.o table.o -lc
trees.o: $M/manifest macdefs $M/mfile1 $M/trees.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/trees.c
optim.o: $M/manifest macdefs $M/mfile1 $M/optim.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/optim.c
pftn.o: $M/manifest macdefs $M/mfile1 $M/pftn.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/pftn.c
code.o: $M/manifest macdefs $M/mfile1
	$(CC) -c $(CFLAGS) -I$M -I. code.c
local.o: $M/manifest macdefs $M/mfile1
	$(CC) -c $(CFLAGS) -I$M -I. local.c
scan.o: $M/manifest macdefs $M/mfile1 $M/scan.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/scan.c
xdefs.o: $M/manifest $M/mfile1 macdefs $M/xdefs.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/xdefs.c
cgram.o: $M/manifest $M/mfile1 macdefs cgram.c
	$(CC) -c $(CFLAGS) -I$M -I. cgram.c
cgram.c: $M/cgram.y
	yacc $M/cgram.y
	mv y.tab.c cgram.c
comm1.o: $M/manifest $M/mfile1 $M/common macdefs $M/comm1.c
	$(CC) -c $(CFLAGS) -I. -I$M $M/comm1.c
table.o: $M/manifest $M/mfile2 mac2defs macdefs table.c
	$(CC) -c -I$M -I. table.c
reader.o: $M/manifest $M/mfile2 mac2defs macdefs $M/reader.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/reader.c
local2.o: $M/manifest $M/mfile2 mac2defs macdefs
	$(CC) -c $(CFLAGS) -I$M -I. local2.c
order.o: $M/manifest $M/mfile2 mac2defs macdefs
	$(CC) -c $(CFLAGS) -I$M -I. order.c
match.o: $M/manifest $M/mfile2 mac2defs macdefs $M/match.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/match.c
allo.o: $M/manifest $M/mfile2 mac2defs macdefs $M/allo.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/allo.c
shrink:
	rm *.o ccom
lintall:
	lint -hpv -I. -I$M  cgram.c xdefs.c $M/scan.c $M/pftn.c $M/trees.c $M/optim.c code.c local.c $M/reader.c local2.c order.c $M/match.c $M/allo.c $M/comm1.c table.c>pcc.lint
fort: ccom fort.o freader.o fallo.o fmatch.o ftable.o forder.o flocal2.o fcomm2.o
	$(CC) -i $(CFLAGS) fort.o freader.o fallo.o fmatch.o ftable.o forder.o flocal2.o fcomm2.o
	mv a.out fort
fort.o: fort.h $M/fort.c
	$(CC) -c $(CFLAGS) -I$M -I. $M/fort.c
freader.o: reader.o
	$(CC) -c $(CFLAGS) -I$M -I. $M/freader.c
fallo.o: allo.o
	$(CC) -c $(CFLAGS) -I$M -I. $M/fallo.c
fmatch.o: match.o
	$(CC) -c $(CFLAGS) -I$M -I. $M/fmatch.c
ftable.o: table.o
	$(CC) -c -I$M -I. $M/ftable.c
forder.o: order.o
	$(CC) -c $(CFLAGS) -I$M -I. $M/forder.c
flocal2.o: local2.o
	$(CC) -c $(CFLAGS) -I$M -I. $M/flocal2.c
fcomm2.o: $M/common
	$(CC) -c $(CFLAGS) -I$M -I. $M/fcomm2.c
fort.o freader.o fallo.o fmatch.o ftable.o forder.o flocal2.o fcomm2.o: $M/mfile2 $M/manifest macdefs mac2defs

