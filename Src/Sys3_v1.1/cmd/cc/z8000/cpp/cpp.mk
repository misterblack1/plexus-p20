#	"@(#)cpp.mk	1.1"
PCC = pcc
CFLAGS = -O
LDFLAGS = -n -s
YACCRM=-rm
IFLAG = -i
V =
INS = :
INSDIR = /lib
CCDIR = /bin
FFLAG =

install: cpp
	/etc/install -o -f $(INSDIR) cpp

cpp::	cpp.o cpy.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o cpp cpp.o cpy.o $(LIBES)

cpp.o: cpp.c
	$(CC) $(CFLAGS) -Dplexus -Dz8000 -Updp11 -c cpp.c

cpy.o:	cpy.c yylex.c
	$(CC) $(CFLAGS) -c cpy.c

cpp68.o: cpp.c
	$(CC) $(CFLAGS) -Dplexus -Dm68 -Dunix -Updp11 -Uz8000 -c cpp.c

cpp68:	cpp68.o cpy.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o cpp68 cpp68.o cpy.o $(LIBES)


