CC = cc

LIB = ../lib/rje.a

install:	directs rjeqer rjeinit rjexmit rjerecv \
	rjedisp rjehalt shqer rjeload

install:
	cp lines /usr/rje/lines
	-chmod 644 /usr/rje/lines
	-chown rje /usr/rje/lines

rjeload:
	cp rjeload.sh /usr/rje/rjeload
	-chmod 500 /usr/rje/rjeload
	-chown rje /usr/rje/rjeload

rjeqer:		rjeqer.c $(LIB)
	$(CC) -s -O -Dz8000 -DVPMSYS -n -o rjeqer rjeqer.c $(LIB)
	cp rjeqer /usr/rje/rjeqer
	-chmod 4755 /usr/rje/rjeqer
	-chown rje /usr/rje/rjeqer

rjeinit:	rjeinit.c $(LIB)
	$(CC) -s -O -Dz8000 -DVPMSYS -n -o rjeinit rjeinit.c $(LIB)
	cp rjeinit /usr/rje/rjeinit
	-chmod 755 /usr/rje/rjeinit
	-chown rje /usr/rje/rjeinit

rjexmit:	rjexmit.c $(LIB)
	$(CC) -s -O -Dz8000 -DVPMSYS -n -o rjexmit rjexmit.c $(LIB)
	cp rjexmit /usr/rje/rjexmit
	-chmod 755 /usr/rje/rjexmit
	-chown rje /usr/rje/rjexmit

rjerecv:	rjerecv.c $(LIB)
	$(CC) -s -O -Dz8000 -DVPMSYS -n -o rjerecv rjerecv.c $(LIB)
	cp rjerecv /usr/rje/rjerecv
	-chmod 755 /usr/rje/rjerecv
	-chown rje /usr/rje/rjerecv

rjedisp:	rjedisp.c $(LIB)
	$(CC) -s -O -Dz8000 -DVPMSYS -n -o rjedisp rjedisp.c $(LIB)
	cp rjedisp /usr/rje/rjedisp
	-chmod 755 /usr/rje/rjedisp
	-chown rje /usr/rje/rjedisp

rjehalt:	rjehalt.c $(LIB)
	$(CC) -s -O -Dz8000 -DVPMSYS -n -o rjehalt rjehalt.c $(LIB)
	cp rjehalt /usr/rje/rjehalt
	-chmod 755 /usr/rje/rjehalt
	-chown rje /usr/rje/rjehalt

shqer:		shqer.c $(LIB)
	$(CC) -s -O -Dz8000 -DVPMSYS -n -o shqer shqer.c $(LIB)
	cp shqer /usr/rje/shqer
	-chmod 4755  /usr/rje/shqer
	-chown root /usr/rje/shqer

$(LIB):		../lib/*.c
	cd ../lib; make -f lib.mk rje.a clean

directs:
	-mkdir /usr/rje
	-mkdir /usr/rje/sque
	-chmod 755 /usr/rje
	-chown rje /usr/rje
	-chmod 755 /usr/rje/sque
	-chown rje /usr/rje/sque

clean:
	-rm -f rjeqer rjeinit rjexmit rjerecv rjedisp
	-rm -f rjehalt shqer
