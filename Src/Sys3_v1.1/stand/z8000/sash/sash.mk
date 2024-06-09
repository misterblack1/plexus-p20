sash:	sash0.o sash1.o
	ld -o sash sash0.o sash1.o
	rm -r sash0.o sash1.o
	$(INS) -f $(ROOT)/stand sash

sash0.o:	sash.s ../z8001.s
	/usr/lib/cpp -I$(INCRT) -P $< >tempfile
	as -u tempfile -o $@
	rm tempfile

sash1.o:	sash1.c
	scc -c sash1.c

clobber:
	rm -f *.o

clean:	clean
	rm -f sash
	
