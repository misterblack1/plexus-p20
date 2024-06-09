#	"@(#)makefile	1.1"
CC=cc
CFLAGS=-O
c2: c20.o c21.o c22.o c23.o
	cc -i -O -X -o c2 c20.o c21.o c22.o c23.o

c2a: c20quick1.o c21quick1.o c22quick1.o c23quick1.o
	cc -i -O -X -o c2a c20quick1.o c21quick1.o c22quick1.o c23quick1.o

all:	c2 c2a

c20.o:	c2.h

c20quick1.o: c2.h c20.c

c21quick1.o: c21.c

c22quick1.o: c22.c

c23quick1.o: c23.c

lint:
	lint c20.c c21.c c22.c  c23.c >c2.lint
