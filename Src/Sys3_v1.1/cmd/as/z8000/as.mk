# Description file for the Z8000 assembler.

INS = /etc/install
LDFLAGS = -s
IFLAG = -i
CFLAGS = -O
OBJECTS = expr.o ifile.o inst.o ofile.o\
	oper.o pass1.o pass2.o scan.o table1.o table2.o z8000.o 

all:	as

install: all
	$(INS) -n /bin as

clean:
	rm -f *.o 

clobber:
	rm -f as

as:	$(OBJECTS) symbols.o opcodes.o
	$(CC) $(CFLAGS) -o as $(IFLAG) $(LDFLAGS) symbols.o opcodes.o \
		$(OBJECTS)

xas:	$(OBJECTS) symbols.o opcodes.o
	$(CC) $(CFLAGS) -o as $(IFLAG) symbols.o opcodes.o \
		$(OBJECTS)


prof:	$(OBJECTS) symbols.o opcodes.o
	$(CC) $(CFLAGS) -p -o as $(IFLAG) $(LDFLAGS) symbols.o opcodes.o \
		$(OBJECTS)

linkas:
	$(CC) $(CFLAGS) -o as $(IFLAG) $(LDFLAGS) symbols.o opcodes.o \
		$(OBJECTS)

cross:	$(OBJECTS) zsymbols.o zopcodes.o
	$(CC) $(CFLAGS) -o ascross $(IFLAG) $(LDFLAGS) zsymbols.o \
		zopcodes.o $(OBJECTS)

$(OBJECTS):	class.h common.h condcode.h crossmode.h\
	entries.h errmsg.h error.h extern.h extern1.h\
	extern2.h format.h ifile.h index.h mask.h mode.h\
	opcode.h operand.h scanner.h symbol.h\
	symtab.h token.h types.h 

