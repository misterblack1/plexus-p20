INCRT = $(ROOT)/usr/include
CFLAGS = -O -DICP -DVPR -Dz8000 -I$(INCRT)
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
	/lib/cpp -DASSM -DICP -DVPR -Dz8000 -I$(INCRT) -P  $< >tempfile
	as -u tempfile -o $@
	rm tempfile
	$(FRC)

mch.o:\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/sioccsv.h
	/lib/cpp -DASSM -DICP -DVPR -Dz8000 -I$(INCRT) -P  $< >tempfile
	as -u tempfile -o $@
	rm tempfile
	$(FRC)

print:
	lnum ../cf/*.[cs] > /dev/lp

FRC:
