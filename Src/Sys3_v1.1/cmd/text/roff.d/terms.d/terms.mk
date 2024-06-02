#	nroff terminal driving tables make file
#	SCCS: @(#)terms.mk	1.14

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/roff.d/terms.d
REL = current
TSID = -r`gsid terms ${REL}`
CSID = -r`gsid maketerms ${REL}`
MKSID = -r`gsid terms.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
INSDIR = ${OL}/lib/term
TFILES = code.300 code.lp tab2631.c tab2631-c.c tab2631-e.c tab300.c \
	 tab300-12.c tab300s.c tab300s-12.c tab37.c tab382.c \
	 tab4000A.c tab450.c tab450-12.c tab832.c taba1.c tablp.c tabtn300.c \
	 tabX.c
PFILES = maketerms.c terms.hd
FILES = tab2631 tab2631-c tab2631-e tab300 tab300-12 tab300s \
	tab300s-12 tab37 tab382 tab4000A tab450 \
	tab450-12 tab832 taba1 tablp tabtn300 tabX

maketerms compile all:  terms.hd
	${CC} -DPART1 -o maketerms maketerms.c
	./maketerms
	${CC} -DPART2 -o maketerms maketerms.c
	./maketerms
	${CC} -DPART3 -o maketerms maketerms.c
	./maketerms

install:  maketerms
	cp ${FILES} ${INSDIR}
	cd ${INSDIR}; rm -f tab300S tab300S-12
	cd ${INSDIR};  ln tab300s tab300S;  ln tab300s-12 tab300S-12
	cd ${INSDIR}; chmod 644 ${FILES}
	-if [ "${OL}" = "/usr" ]; \
		then cd ${INSDIR}; chown bin ${FILES} tab300S tab300S-12; \
			chgrp bin ${FILES} tab300S tab300S-12; \
	 fi

build:  bldmk bldt bldmt
	:
bldt:	;  get -p ${TSID} s.terms.src | ntar -d ${RDIR} -g
bldmt:	;  get -p ${CSID} s.maket.src | ntar -d ${RDIR} -g
bldmk: ; get -p ${MKSID} s.terms.mk > ${RDIR}/terms.mk

listing:
	pr terms.mk ${TFILES} ${PFILES} | ${LIST}
listmk: ;  pr terms.mk | ${LIST}
listt:	;  pr ${TFILES} | ${LIST}
listmt:	;  pr ${PFILES} | ${LIST}

edit:	mkedit tedit mtedit
	:
mkedit:	;  get -e s.terms.mk
tedit:	;  get -e -p s.terms.src | ntar -g
mtedit:	;  get -e -p s.maket.src | ntar -g

delta:	mkdelta tdelta mtdelta
	:
mkdelta:  ;  delta s.terms.mk
tdelta:   ;  ntar -p ${TFILES} > terms.src
	     delta s.terms.src
	     rm -f ${TFILES}
mtdelta:  ;  ntar -p ${PFILES} > maket.src
	     delta s.maket.src
	     rm -f ${PFILES}

clean:
	rm -f maketerms
clobber:  clean
	rm -f ${FILES}
delete:	clobber
	rm -f ${TFILES} ${PFILES}
