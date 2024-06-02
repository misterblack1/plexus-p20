CC = cc
CCFLAGS = -O -I/p3/usr/include

LD = -ld
LDFLAGS = -x

.c.o:
	$(CC) -c $(CCFLAGS) $<
	$(LD) -r $(LDFLAGS) $*.o
	mv a.out $*.o

OBJ1 = chkid.o chksid.o del_ba.o  date_ba.o dodelt.o
OBJ2 = del_ab.o dofile.o dohist.o doie.o dolist.o eqsid.o flushto.o newstats.o
OBJ3 = permiss.o logname.o pf_ab.o date_ab.o rdmod.o setup.o
OBJ4 = sid_ab.o sid_ba.o sidtoser.o sinit.o stats_ab.o
OBJ5 = fmterr.o getline.o putline.o auxf.o

comobj.a:	$(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5)
	-rm -f comobj.a tmp.a
	ar r tmp.a $(OBJ1)
	ar r tmp.a $(OBJ2)
	ar r tmp.a $(OBJ3)
	ar r tmp.a $(OBJ4)
	ar r tmp.a $(OBJ5)
	-chown bin tmp.a
	-chgrp bin tmp.a
	-chmod 644 tmp.a
	mv tmp.a comobj.a

clean:
	-rm -f $(OBJ1)
	-rm -f $(OBJ2)
	-rm -f $(OBJ3)
	-rm -f $(OBJ4)
	-rm -f $(OBJ5)

clobber:	clean
