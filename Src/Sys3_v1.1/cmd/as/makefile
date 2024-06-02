# Description file for the Z8000 assembler.

CFLAGS = -O
OBJECTS = expr.o ifile.o inst.o ofile.o\
	oper.o pass1.o pass2.o scan.o table1.o table2.o z8000.o 

all:	as

cp:	as
	strip as
	cp as /bin/as
	chmod 0775 /bin/as; chown 3 /bin/as; chgrp 3 /bin/as
	rm *.o as

xcp:	as
	cp as /bin/as
	rm *.o as

cmp:	as
	strip as
	cmp as /bin/as
	rm *.o as

as:	$(OBJECTS) symbols.o opcodes.o
	$(CC) $(CFLAGS) -o as -i symbols.o opcodes.o $(OBJECTS)

prof:	$(OBJECTS) symbols.o opcodes.o
	$(CC) $(CFLAGS) -p -o as -i symbols.o opcodes.o $(OBJECTS)

linkas:
	$(CC) $(CFLAGS) -o as -i symbols.o opcodes.o $(OBJECTS)

cross:	$(OBJECTS) zsymbols.o zopcodes.o
	$(CC) $(CFLAGS) -o ascross -i zsymbols.o zopcodes.o $(OBJECTS)

$(OBJECTS):	class.decs common.decs condcode.decs crossmode.decs\
	entries.decs errmsg.decs error.decs extern.decs extern1.decs\
	extern2.decs format.decs ifile.decs index.decs mask.decs mode.decs\
	opcode.decs operand.decs scanner.decs symbol.decs\
	symtab.decs token.decs types.decs 

