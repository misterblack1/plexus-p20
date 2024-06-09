#	neqn make file (text subsystem)
#	SCCS:   @(#)neqn.mk	1.12

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/neqn.d
INS = :
REL = current
CSID = -r`gsid neqn ${REL}`
MKSID = -r`gsid neqn.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}/bin
CFLAGS = -O -DNEQN
IFLAG = -n
YFLAGS = -d
SOURCE = e.y e.h diacrit.c eqnbox.c font.c fromto.c funny.c glob.c integral.c \
	 io.c lex.c lookup.c mark.c matrix.c move.c over.c paren.c \
	 pile.c shift.c size.c sqrt.c text.c
FILES =  diacrit.o eqnbox.o font.o fromto.o funny.o glob.o integral.o \
	 io.o lex.o lookup.o mark.o matrix.o move.o over.o paren.o \
	 pile.o shift.o size.o sqrt.o text.o e.o
MAKE = make

compile all:	neqn
	:

neqn:	$(FILES)
	$(CC) $(LDFLAGS) ${IFLAG} ${FFLAG} -o neqn $(FILES) -ly
	$(INS) neqn $(CINSDIR)
	chmod 755 ${CINSDIR}/neqn
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin neqn; chgrp bin neqn; \
	 fi

$(FILES):: e.h e.def
	:

e.def:    y.tab.h
	  -cmp -s y.tab.h e.def || cp y.tab.h e.def

y.tab.h:  e.o
	:

install:
	${MAKE} -f neqn.mk INS=cp OL=${OL} SL=${SL} LDFLAGS="$(LDFLAGS)"

build:	bldmk
	get -p ${CSID} s.neqn.src ${REWIRE} | ntar -d ${RDIR} -g
bldmk: ; get -p ${MKSID} s.neqn.mk > ${RDIR}/neqn.mk

listing:
	pr neqn.mk ${SOURCE} | ${LIST}
listmk: ; pr neqn.mk | ${LIST}

edit:
	get -e -p s.neqn.src | ntar -g

delta:
	ntar -p ${SOURCE} > neqn.src
	delta s.neqn.src
	rm -f ${SOURCE}

mkedit:  ;  get -e s.neqn.mk
mkdelta: ;  delta s.neqn.mk

clean:
	rm -f *.o y.tab.[ch] e.def

clobber:  clean
	rm -f neqn

delete:	clobber
	rm -f ${SOURCE}
