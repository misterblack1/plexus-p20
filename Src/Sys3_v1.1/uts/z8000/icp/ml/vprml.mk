INCRT = $(ROOT)/usr/include
CFLAGS = -O -DICP -DVPR -Dz8000 -I$(INCRT)
FRC =

FILES =\
	clist.o\
	cinit.o

all:	$(FILES)

clist.o:
	/lib/cpp -DASSM -DICP -DVPR -Dz8000 -I$(INCRT) -P  $< >tempfile
	as -u tempfile -o $@
	rm tempfile
	$(FRC)

cinit.o:\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/tty.h
	cc $(CFLAGS) -c $<
	$(FRC)

print:
	lnum ../ml/*.[cs] > /dev/lp

FRC:
