ULIB = /usr/lib
CFLAGS = -O
LNAME = libPW.a

OBJ1 = anystr.o bal.o curdir.o fdfopen.o giveup.o
OBJ2 = imatch.o index.o lockit.o logname.o move.o patoi.o
OBJ3 = patol.o regcmp.o regex.o rename.o repeat.o repl.o satoi.o
OBJ4 = setsig.o sname.o strend.o substr.o trnslat.o userdir.o
OBJ5 = username.o verify.o any.o xalloc.o xcreat.o xlink.o
OBJ6 = xopen.o xpipe.o xunlink.o xwrite.o xmsg.o alloca.o
OBJ7 = cat.o dname.o fatal.o clean.o userexit.o zero.o zeropad.o

init:
	cp z8000/move.c move.c
	cp z8000/alloca.s alloca.s

$(LNAME):	init $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7)
	-rm -f $(LNAME)
	ar r $(LNAME) $(OBJ1)
	ar r $(LNAME) $(OBJ2)
	ar r $(LNAME) $(OBJ3)
	ar r $(LNAME) $(OBJ4)
	ar r $(LNAME) $(OBJ5)
	ar r $(LNAME) $(OBJ6)
	ar r $(LNAME) $(OBJ7)
	strip $(LNAME)

.s.o:
	/lib/cpp -P $< >tempfile
	as -u tempfile -o $@
	rm tempfile

install:	$(LNAME)
	/etc/install -n $(ULIB) $(LNAME)

clean:
	-rm -f $(OBJ1)
	-rm -f $(OBJ2)
	-rm -f $(OBJ3)
	-rm -f $(OBJ4)
	-rm -f $(OBJ5)
	-rm -f $(OBJ6)
	-rm -f $(OBJ7)
	-rm -f move.c move.s alloca.s $(LNAME)

clobber:	clean
