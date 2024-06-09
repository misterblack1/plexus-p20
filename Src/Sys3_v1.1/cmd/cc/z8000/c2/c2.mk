
#	"@(#)makefile	1.1"
INSDIR = /lib
CC=cc
CFLAGS=-O

install:c2 c2a
	/etc/install -f $(INSDIR) c2
	/etc/install -f $(INSDIR) c2a

c2: c20.o c21.o c22.o c23.o
	cc -i -O -X -o c2 c20.o c21.o c22.o c23.o

c2a: c20a.o c21a.o c22a.o c23a.o
	cc -i -O -X -o c2a c20a.o c21a.o c22a.o c23a.o

kc2: c21.o c22.o c23.o
	cc -i -O -X -o kc2 -DSAVECSV c20.c c21.o c22.o c23.o

all:	c2 c2a

c20.o:	c2.h

c20a.o: c2.h c20.c

c21a.o: c21.c

c22a.o: c22.c

c23a.o: c23.c

lint:
	lint c20.c c21.c c22.c  c23.c >c2.lint
