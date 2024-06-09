#	eqn make file (text subsystem)
#	SCCS:  @(#)eqn.mk	1.11

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/eqn.d
INS = :
REL = current
CSID = -r`gsid eqn ${REL}`
MKSID = -r`gsid eqn.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}/bin
CFLAGS = -O
IFLAG = -n
YFLAGS = -d
SOURCE = e.y e.h diacrit.c eqnbox.c font.c fromto.c funny.c glob.c integral.c \
	 io.c lex.c lookup.c mark.c matrix.c move.c over.c paren.c \
	 pile.c shift.c size.c sqrt.c text.c
FILES =  diacrit.o eqnbox.o font.o fromto.o funny.o glob.o integral.o \
	 io.o lex.o lookup.o mark.o matrix.o move.o over.o paren.o \
	 pile.o shift.o size.o sqrt.o text.o e.o
MAKE = make

compile all: eqn
	:

eqn:	$(FILES)
	$(CC) $(LDFLAGS) ${IFLAG} ${FFLAG} -o eqn $(FILES) -ly
	$(INS) eqn $(CINSDIR)
	chmod 755 ${CINSDIR}/eqn
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin eqn; chgrp bin eqn; \
	 fi

$(FILES):: e.h e.def
	:

e.def:	  y.tab.h
	  -cmp -s y.tab.h e.def || cp y.tab.h e.def

y.tab.h:  e.o
	:

install:
	${MAKE} -f eqn.mk INS=cp OL=${OL} LDFLAGS="$(LDFLAGS)" SL=${SL}

build:	bldmk
	get -p ${CSID} s.eqn.src ${REWIRE} | ntar -d ${RDIR} -g
bldmk:  ;  get -p ${MKSID} s.eqn.mk > ${RDIR}/eqn.mk

listing:
	pr eqn.mk ${SOURCE} | ${LIST}
listmk: ;  pr eqn.mk | ${LIST}

edit:
	get -e -p s.eqn.src | ntar -g

delta:
	ntar -p ${SOURCE} > eqn.src
	delta s.eqn.src
	rm -f ${SOURCE}

mkedit:  ;  get -e s.eqn.mk
mkdelta: ;  delta s.eqn.mk

clean:
	  rm -f *.o y.tab.h e.def

clobber:  clean
	  rm -f eqn

delete:	clobber
	rm -f ${SOURCE}
