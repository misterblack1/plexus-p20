CFLAGS=-O

cp:	c2
	strip c2
	mv c2 /lib/c2
	chmod 775 /lib/c2; chown 3 /lib/c2; chgrp 3 /lib/c2
	-rm -f *.o c2

xcp:	c2
	mv c2 /lib/c2
	rm *.o c2

cmp:	c2
	strip c2
	cmp c2 /lib/c2
	rm *.o c2

c2: c20.o c21.o c22.o c23.o
	cc -i -O -X -o c2 c20.o c21.o c22.o c23.o

c2small: c20small.o c21small.o c22small.o c23small.o
	cc -i -O -X -o c2small c20small.o c21small.o c22small.o c23small.o

c20small.o: c20.c

c21small.o: c21.c

c22small.o: c22.c

c23small.o: c23.c

lint:
	lint c20.c c21.c c22.c  c23.c >c2.lint
