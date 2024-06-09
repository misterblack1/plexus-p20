TESTDIR = .
I = /etc/install
INSDIR =
LIBNAME = a.a
CFLAGS = -O
INCLUDE = /usr/include
INCLUDEDIR = ..
FRC =

FILES =	pnpsplit.x copyn.x devtolin.x expand.x lintodev.x namtouid.x\
	tmless.x tmsecs.x uidtonam.x\
	substr.x cat.x regex.x regcmp.x

all:	$(TESTDIR)/$(LIBNAME) $(FRC)

$(TESTDIR)/$(LIBNAME):	$(FILES)

install:	all
	$(I) $(LIBNAME)

clean:
	-rm -f *.o
	-rm -f a.a

clobber:	clean
	-rm -f *.x


FRC:

.SUFFIXES:	.o .x .s .c
.c.o:
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -I$(INCLUDE) -c $<
	mv $*.o a.out
	-ld -r -x -o $*.o a.out
	-rm -f a.out

.c.x:
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -I$(INCLUDE) -c $<
	mv $*.o a.out
	-ld -r -x -o $*.o a.out
	ar ru $(TESTDIR)/$(LIBNAME) $*.o
	-rm $*.o a.out
	> $*.x
.s.o:
	$(AS) $<
	-ld -r -x -o $*.o a.out
	-rm -f a.out

.s.x:
	$(AS) $<
	-ld -r -x -o $*.o a.out
	ar ru $(TESTDIR)/$(LIBNAME) $*.o
	-rm $*.o a.out
	> $*.x

