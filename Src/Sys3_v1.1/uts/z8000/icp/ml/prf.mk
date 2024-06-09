INCRT = $(ROOT)/usr/include
CFLAGS = -O -DICP -Dz8000 -DPROFILE -I$(INCRT)
FRC =

FILES =\
	clist.o\
	cinit.o

all:	$(FILES)

clist.o:
	/lib/cpp -DASSM -DICP -Dz8000 -I$(INCRT) -P  $< >tempfile
	as -u tempfile -o $@
	rm tempfile
	$(FRC)

cinit.o:\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/tty.h
	cc $(CFLAGS) -c $<
	$(FRC)

FRC:
