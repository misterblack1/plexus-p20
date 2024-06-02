#	nroff/troff make file (text subsystem)
#	SCCS:   @(#)roff.mk	1.17

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/roff.d
INS = :
ARGS = all
REL = current
MKSID = -r`gsid roff.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}/bin
DINSDIR = ${OL}/lib
MAKE = make

compile all:  nroff troff suftab term font
	:

nroff:
	${MAKE} nroff clean SL=${SL} LDFLAGS="$(LDFLAGS)" OL=${OL}
	${INS} nroff ${CINSDIR}
	chmod 755 ${CINSDIR}/nroff
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin nroff; chgrp bin nroff; \
	 fi

troff:
	${MAKE} PTAG= troff clean SL=${SL} LDFLAGS="$(LDFLAGS)" OL=${OL}
	${INS} troff ${CINSDIR}
	chmod 755 ${CINSDIR}/troff
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin troff; chgrp bin troff; \
	 fi

suftab:
	${MAKE} suftab SL=${SL} OL=${OL} LDFLAGS="$(LDFLAGS)"
	${INS} suftab ${DINSDIR}
	chmod 644 ${DINSDIR}/suftab
	-if [ "${OL}" = "/usr" ]; \
		then cd ${DINSDIR}; chown bin suftab; chgrp bin suftab; \
	 fi

term:
	cd terms.d; ${MAKE} -f terms.mk OL=${OL} ${ARGS} SL=${SL} \
		LDFLAGS="$(LDFLAGS)"
font:
	cd fonts.d; ${MAKE} -f fonts.mk OL=${OL} ${ARGS} SL=${SL} \
		LDFLAGS="$(LDFLAGS)"

listing:  listmk listnroff listterms listfonts
	:
listmk:     ;  pr roff.mk | ${LIST}
listnroff listtroff:  ;  ${MAKE} -i LIST="${LIST}" listing
listterms:  ;  cd terms.d; ${MAKE} -i -f terms.mk LIST="${LIST}" listing
listfonts:  ;  cd fonts.d; ${MAKE} -i -f fonts.mk LIST="${LIST}" listing

build:  bldmk bldnroff bldterms bldfonts
	:
bldmk:   ; get -p ${MKSID} s.roff.mk > ${RDIR}/roff.mk
bldnroff bldtroff:
	${MAKE} RDIR=${RDIR} REL=${REL} REWIRE='${REWIRE}' build
bldterms:
	cd terms.d; ${MAKE} -f terms.mk RDIR=${RDIR}/terms.d \
			REL=${REL} REWIRE='${REWIRE}' build
bldfonts:
	cd fonts.d; ${MAKE} -f fonts.mk RDIR=${RDIR}/fonts.d \
			REL=${REL} REWIRE='${REWIRE}' build

install:
	${MAKE} -f roff.mk INS=cp OL=${OL} SL=${SL} ARGS=install ${ARGS} \
		LDFLAGS="$(LDFLAGS)"
insnroff:  ;  ${MAKE} -f roff.mk INS=cp OL=${OL} nroff SL=${SL} \
		LDFLAGS="$(LDFLAGS)"
instroff:  ;  ${MAKE} -f roff.mk INS=cp OL=${OL} troff SL=${SL} \
		LDFLAGS="$(LDFLAGS)"

edit:	mkedit
	${MAKE} edit
delta:	mkdelta
	${MAKE} delta

mkedit:	;  get -e s.roff.mk
mkdelta: ; delta s.roff.mk

ntedit:	;  ${MAKE} ntedit
ntdelta: ; ${MAKE} ntdelta

bedit:	;  ${MAKE} bedit
bdelta:	;  ${MAKE} bdelta

makeedit:  ;  ${MAKE} makeedit
makedelta: ;  ${MAKE} makedelta

clean:	nclean
	cd terms.d; ${MAKE} -f terms.mk clean
	cd fonts.d; ${MAKE} -f fonts.mk clean
nclean:	;  ${MAKE} clean

clobber: nclobber
	cd terms.d; ${MAKE} -f terms.mk clobber
	cd fonts.d; ${MAKE} -f fonts.mk clobber
nclobber: ;  ${MAKE} clobber
bclobber: ;  ${MAKE} bclobber

delete:	ndelete
	cd terms.d; ${MAKE} -f terms.mk delete
	cd fonts.d; ${MAKE} -f fonts.mk delete
ndelete: ;  ${MAKE} delete
bdelete: ;  ${MAKE} bdelete
