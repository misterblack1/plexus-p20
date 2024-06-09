CFLAGS =-O
LDFLAGS=
IFLAG=-i
INS=/etc/install
INSDIR=/bin

all:	adb

clean:
	rm  -f *.o

clobber:
	rm -f adb

install: all
	$(INS) -n $(INSDIR) adb

adb:	access.o command.o expr.o findfn.o format.o input.o opset.o \
	main.o message.o output.o pcs.o print.o runpcs.o setup.o sym.o
	cc $(CFLAGS) $(LDFLAGS) $(IFLAG) -o adb  *.o

access.o:	defs.h
command.o:	defs.h
expr.o:		defs.h
findfn.o:	defs.h
format.o:	defs.h
input.o:	defs.h
main.o:		defs.h
message.o:	mac.h mode.h
opset.o:	d4.h
output.o:	defs.h
pcs.o:		defs.h
print.o:	defs.h
runpcs.o:	defs.h
setup.o:	defs.h
sym.o:		defs.h
