CFLAGS = -O -DICP -Dpdp11
OBJS =	main.o\
	prf.o\
	trap.o\
	mb.o\
	sio.o\
	mch.o\
	l.o
.s.o:
	/lib/cpp -DASSM -DICP -P  $< >tempfile
	as -u tempfile -o $@
	rm tempfile

all: $(OBJS)
