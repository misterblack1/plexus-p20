#	text Development Support Library (DSL) tbl make file
#	SCCS: @(#)tbl.mk	1.12

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/tbl.d
INS = :
REL = current
CSID = -r`gsid tbl ${REL}`
MKSID = -r`gsid tbl.mk ${REL}`
CINSDIR = ${OL}/bin
BIN = P108
LIST = opr -ttx -b${BIN}
CFLAGS = -O
SFILES = t..c t[0-9].c t[bcefgimrstuv].c
OFILES = t0.o t1.o t2.o t3.o t4.o t5.o t6.o t7.o t8.o t9.o tb.o tc.o\
	te.o tf.o tg.o ti.o tm.o tr.o ts.o tt.o tu.o tv.o
IFLAG = -i
MAKE = make

compile all: tbl
	:

tbl:	$(OFILES) 
	$(CC) $(LDFLAGS) $(IFLAG) $(CFLAGS) ${FFLAG} -o tbl $(OFILES)
	${INS} tbl ${CINSDIR}
	chmod 755 ${CINSDIR}/tbl
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin tbl; chgrp bin tbl; \
	 fi

$(OFILES):: t..c
	:

install:
	${MAKE} -f tbl.mk INS=cp OL=${OL} SL=${SL} LDFLAGS="$(LDFLAGS)"

build:	bldmk
	get -p ${CSID} s.tbl.src ${REWIRE} | ntar -d ${RDIR} -g
bldmk: ; get -p ${MKSID} s.tbl.mk > ${RDIR}/tbl.mk

listing:
	pr tbl.mk ${SFILES} | ${LIST}
listmk:  ;  pr tbl.mk | ${LIST}

edit:
	get -p -e s.tbl.src | ntar -g

delta:
	ntar -p ${SFILES} > tbl.src
	delta s.tbl.src
	rm -f ${SFILES}

mkedit:	;  get -e s.tbl.mk
mkdelta: ; delta s.tbl.mk

clean:
	rm -f *.o

clobber: clean
	rm -f tbl

delete:	clobber
	rm -f ${SFILES}
