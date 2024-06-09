TESTDIR = .
INS = /etc/install
INSDIR =
FRC =
CFLAGS = -O
LDFLAGS = -s
YACCRM = -rm
OFILES = ctype.o input.o output.o string.o xrefb.o

all:	xref xrefa xrefb

xref:	xref.sh $(FRC)
	cp xref.sh $(TESTDIR)/xref

xrefa:	xrefa.l.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TESTDIR)/xrefa xrefa.l.o -ll

xrefa.l.o:  xrefa.l.c $(FRC)
	$(CC) $(CFLAGS) -c xrefa.l.c

xrefa.l.c: xrefa.l $(FRC)
	$(LEX) $(LFLAGS) xrefa.l
	mv lex.yy.c xrefa.l.c

xrefb:	$(OFILES) divide.o
	$(CC) $(LDFLAGS) -o $(TESTDIR)/xrefb $(OFILES) divide.o

ctype.o xrefb.o:	ctype.h $(FRC)
output.o xrefb.o:	output.h $(FRC)
input.o xrefb.o:	input.h $(FRC)
string.o:		defs.h $(FRC)
output.o input.h:	defs.h $(FRC)
divide.o::	divide.vax.s $(FRC)
	-if vax; then $(AS) -o divide.o divide.vax.s;\
	else	: ; fi
divide.o::	divide.pdp11.s $(FRC)
	-if pdp11; then $(AS) - -o divide.o divide.pdp11.s;\
	else	: ; fi
divide.o::	divide.z8000.c $(FRC)
	$(CC) -c $(CFLAGS) -o divide.z8000.o divide.z8000.c

test:
	  rtest $(TESTDIR)/xref

install:  all
	  $(INS) -n /usr/bin $(TESTDIR)/xref
	  $(INS) -n /usr/lib $(TESTDIR)/xrefa $(INSDIR)
	  $(INS) -n /usr/lib $(TESTDIR)/xrefb $(INSDIR)

clean:
	  -rm -f *.o

clobber:  clean
	  -cd $(TESTDIR); rm -f xref xrefa xrefb
	$(YACCRM) -f xrefa.l.c

FRC:
