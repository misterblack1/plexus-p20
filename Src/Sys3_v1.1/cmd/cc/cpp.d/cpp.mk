
CFLAGS = -O
LDFLAGS = -n -s
YACCRM=-rm
INS = /etc/install
INSDIR = /lib
FFLAG =

all:	cpp

cpp:	cpp.o cpy.o
	$(CC) $(LDFLAGS) -o cpp cpp.o cpy.o

cpp.o:	cpp.c
#		The "-DPWB" added to obviate a V7 compiler bug
	$(CC) -DPWB $(CFLAGS) -c cpp.c

install: all
	$(INS) cpp $(INSDIR)

clean:
	rm -f *.o

clobber: clean
	rm -f cpp

cpy.o:	cpy.c yylex.c
	$(CC) $(CFLAGS) -c cpy.c
cpy.c:	cpy.y


