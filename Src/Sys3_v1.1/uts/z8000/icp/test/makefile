CFLAGS = -O -DICP -Dpdp11 -I/p3/usr/include 
OBJS = main.o\
       getnew.o\
       getold.o\
       setnew.o\
       setold.o

all:	$(OBJS)
	cc main.o getnew.o getold.o setnew.o setold.o -o test
	cp test /p3/sioc/test/test
