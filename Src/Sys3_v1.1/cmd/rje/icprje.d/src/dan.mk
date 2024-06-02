CC = cc

LIB = ../lib/rje.a

rjehalt:	rjehalt.c $(LIB)
	$(CC) -s -O -Dz8000 -DVPMSYS -n -o rjehalt rjehalt.c $(LIB)
	cp rjehalt /usr/rje/rjehalt
	-chmod 755 /usr/rje/rjehalt
	-chown rje /usr/rje/rjehalt

