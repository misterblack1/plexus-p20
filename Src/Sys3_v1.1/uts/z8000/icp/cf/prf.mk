INCRT = $(ROOT)/usr/include
CFLAGS = -O -DICP -Dz8000 -I$(INCRT) -DPROFILE
FRC =

FILES =\
	c.o\
	l.o\
	mch.o

all:	$(FILES)

c.o:\
	$(INCRT)/sys/conf.h
	cc $(CFLAGS) -c $<
	$(FRC)

l.o:\
	$(INCRT)/icp/sioc.h
	/lib/cpp -DASSM -DICP -Dz8000 -DPROFILE  -I$(INCRT) -P  $< >tempfile
	as -u tempfile -o $@
	rm tempfile
	$(FRC)

mch.o:\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/sioccsv.h
	/lib/cpp -DASSM -DICP -Dz8000 -DPROFILE  -I$(INCRT) -P  $< >tempfile
	as -u tempfile -o $@
	rm tempfile
	$(FRC)

print:
	lnum ../cf/*.[cs] > /dev/lp

FRC:
