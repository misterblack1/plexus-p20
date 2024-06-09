CC = cc
CCFLAGS = -O -Dz8000 -DVPMSYS

LD = -ld
LDFLAGS = -x

.c.o:
	$(CC) -c $(CCFLAGS) $<
	$(LD) -r $(LDFLAGS) $*.o
	mv a.out $*.o


AR = ar

#------
# librje.a:	Create librje.a in local directory.
#------
librje.a:	alias.o lookup.o mtch.o order.o \
		prf.o rjesys.o rjetab.o scan.o \
		cat.o
	-rm -f librje.a tmp.a
	$(AR) r tmp.a alias.o lookup.o mtch.o order.o \
		prf.o rjesys.o rjetab.o scan.o cat.o
	-chmod 644 tmp.a
	mv tmp.a librje.a

#------
# clean: Remove all .o files.
#------
clean:
	-rm -f alias.o lookup.o mtch.o order.o \
		prf.o rjesys.o rjetab.o scan.o cat.o

#------
# Dependencies for individual .o files.
#------
alias.o:	alias.c
lookup.o:	lookup.c
mtch.o:		mtch.c
order.o:	order.c
prf.o:		prf.c
rjesys.o:	rjesys.c
rjetab.o:	rjetab.c
scan.o:		scan.c
cat.o:		cat.c
