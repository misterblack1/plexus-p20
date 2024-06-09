#	text subsystem shells make file
#	SCCS:  @(#)shells.mk	1.14

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/shells.d
INS = :
REL = current
MKSID = -r`gsid shells.mk ${REL}`
MMSID = -r`gsid mm.sh ${REL}`
MMTSID = -r`gsid mmt.sh ${REL}`
MANSID = -r`gsid man.sh ${REL}`
MCVTSID = -r`gsid mancvt.sh ${REL}`
MVTSID = -r`gsid mvt.sh ${REL}`
ORGSID = -r`gsid org.sh ${REL}`
OSDDSID = -r`gsid osdd.sh ${REL}`
GSID = -r`gsid greek.sh ${REL}`
TERMHSID = -r`gsid termhelp ${REL}`
TEXTHSID = -r`gsid texthelp ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}/bin
HINSDIR = ${OL}/lib/help
FILES = mm.sh mmt.sh man.sh mancvt.sh mvt.sh org.sh osdd.sh greek.sh term text
MAKE = make

compile all:  man mcvt mm mmt mvt osdd greek termh texth
	:

man:	;  cp man.sh man
	${INS} man ${CINSDIR}
	chmod 755 ${CINSDIR}/man
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin man; chgrp bin man; \
	 fi

mcvt:	;  cp mancvt.sh mancvt
	${INS} mancvt ${CINSDIR}
	chmod 755 ${CINSDIR}/mancvt
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin mancvt; chgrp bin mancvt; \
	 fi

mm:	;  cp mm.sh mm
	${INS} mm ${CINSDIR}
	chmod 755 ${CINSDIR}/mm
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin mm; chgrp bin mm; \
	 fi

mmt:	;  cp mmt.sh mmt
	${INS} mmt ${CINSDIR}
	chmod 755 ${CINSDIR}/mmt
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin mmt; chgrp bin mmt; \
	 fi

mvt:	mmt
	rm -f ${CINSDIR}/mvt
	ln ${CINSDIR}/mmt ${CINSDIR}/mvt
	chmod 755 ${CINSDIR}/mvt
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin mvt; chgrp bin mvt; \
	 fi

org:	;  cp org.sh org
	   ${INS} org ${CINSDIR}
	   chmod 755 ${CINSDIR}/org
	   -if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin org; chgrp bin org; \
	    fi
osdd:	;  cp osdd.sh osdd
	   ${INS} osdd ${CINSDIR}
	   chmod 755 ${CINSDIR}/osdd
	   -if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin osdd; chgrp bin osdd; \
	    fi
greek:	;  cp greek.sh greek
	${INS} greek ${CINSDIR}
	chmod 755 ${CINSDIR}/greek
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin greek; chgrp bin greek; \
	 fi

helpdir:
	-mkdir ${OL}/lib/help

termh:	helpdir
	${INS} term ${HINSDIR}
	chmod 644 ${HINSDIR}/term
	-if [ "${OL}" = "/usr" ]; \
	    then cd ${HINSDIR}; chown bin term; chgrp bin term; \
	 fi

texth:	helpdir
	${INS} text ${HINSDIR}
	chmod 644 ${HINSDIR}/text
	-if [ "${OL}" = "/usr" ]; \
	    then cd ${HINSDIR}; chown bin text; chgrp bin text; \
	 fi

install: ;${MAKE} -f shells.mk INS=cp OL=${OL} SL=${SL} LDFLAGS="$(LDFLAGS)"
insmm:	;${MAKE} -f shells.mk INS=cp OL=${OL} mm SL=${SL} LDFLAGS="$(LDFLAGS)"
insmmt:	; ${MAKE} -f shells.mk INS=cp OL=${OL} mmt SL=${SL} LDFLAGS="$(LDFLAGS)"
insman:	; ${MAKE} -f shells.mk INS=cp OL=${OL} man SL=${SL} LDFLAGS="$(LDFLAGS)"
insmcvt: ;${MAKE} -f shells.mk INS=cp OL=${OL} mcvt SL=${SL} LDFLAGS="$(LDFLAGS)"
insmvt:	; ${MAKE} -f shells.mk INS=cp OL=${OL} mvt SL=${SL} LDFLAGS="$(LDFLAGS)"
insorg:	; ${MAKE} -f shells.mk INS=cp OL=${OL} org SL=${SL} LDFLAGS="$(LDFLAGS)"
insosdd: ; ${MAKE} -f shells.mk INS=cp OL=${OL} osdd SL=${SL} LDFLAGS="$(LDFLAGS)"
insgreek: ; ${MAKE} -f shells.mk INS=cp OL=${OL} greek SL=${SL} LDFLAGS="LDFLAGS)"
instermh: ;${MAKE} -f shells.mk INS=cp OL=${OL} termh SL=${SL} LDFLAGS="$(LDFLAGS)"
instexth: ;${MAKE} -f shells.mk INS=cp OL=${OL} texth SL=${SL} LDFLAGS="$(LDFLAGS)"

listing:  ;  pr shells.mk ${FILES}  |  ${LIST}
listmk:    ;  pr shells.mk | ${LIST}
listmm:    ;  pr mm.sh | ${LIST}
listmmt:   ;  pr mmt.sh | ${LIST}
listman:   ;  pr man.sh | ${LIST}
listmcvt:  ;  pr mancvt.sh | ${LIST}
listmvt:   ;  pr -h "linked copy of mmt - mvt" mvt.sh | ${LIST}
listorg:   ;  pr org.sh | ${LIST}
listosdd:  ;  pr osdd.sh | ${LIST}
listgreek: ;  pr greek.sh | ${LIST}
listtermh: ; pr term | ${LIST}
listtexth: ; pr text | ${LIST}

build:  bldmk bldmm bldmmt bldman bldmcvt bldosdd bldgreek bldtermh bldtexth
	:
bldmk:  ;  get -p ${MKSID} s.shells.mk > ${RDIR}/shells.mk
bldmm:	;  get -p ${MMSID} s.mm.sh ${REWIRE} > ${RDIR}/mm.sh
bldmmt bldmvt:	;  get -p ${MMTSID} s.mmt.sh ${REWIRE} > ${RDIR}/mmt.sh
bldman:	;  get -p ${MANSID} s.man.sh ${REWIRE} > ${RDIR}/man.sh
bldmcvt: ; get -p ${MCVTSID} s.mancvt.sh ${REWIRE} > ${RDIR}/mancvt.sh
bldorg:	;  get -p ${ORGSID} s.org.sh ${REWIRE} > ${RDIR}/org.sh
bldosdd: ; get -p ${OSDDSID} s.osdd.sh ${REWIRE} > ${RDIR}/osdd.sh
bldgreek: ; get -p ${GSID} s.greek.sh ${REWIRE} > ${RDIR}/greek.sh
bldtermh: ; get -p ${TERMHSID} s.term > ${RDIR}/term
bldtexth: ; get -p ${TEXTHSID} s.text > ${RDIR}/text

edit:	manedit mcvtedit mmedit mmtedit orgedit osddedit greekedit \
		termhedit texthedit mkedit
	:
manedit:  ;  get -e s.man.sh
mcvtedit: ;  get -e s.mancvt.sh
mmedit:   ;  get -e s.mm.sh
mmtedit mvtedit:  ;  get -e s.mmt.sh
orgedit:  ;  get -e s.org.sh
osddedit: ;  get -e s.osdd.sh
greekedit: ; get -e s.greek.sh
termhedit: ; get -e s.term
texthedit: ; get -e s.text
mkedit:   ;  get -e s.shells.mk

delta:	mandelta mcvtdelta mmdelta mmtdelta orgdelta osdddelta greekdelta \
		termhdelta texthdelta  mkdelta
	:
mandelta:  ;  delta s.man.sh
mcvtdelta: ;  delta s.mancvt.sh
mmdelta:   ;  delta s.mm.sh
mmtdelta mvtdelta:  ;  delta s.mmt.sh
orgdelta:  ;  delta s.org.sh
osdddelta: ;  delta s.osdd.sh
greekdelta: ; delta s.greek.sh
termhdelta: ; delta s.term
texthdelta: ; delta s.text
mkdelta:   ;  delta s.shells.mk

clean:
	:
clobber:  clean manclobber mcvtclobber mmclobber mmtclobber orgclobber \
		osddclobber greekclobber
	:
manclobber:  ;  rm -f man
mcvtclobber: ;  rm -f mancvt
mmclobber:   ;  rm -f mm
mmtclobber mvtclobber:  ;  rm -f mmt
orgclobber:  ;  rm -f org
osddclobber: ;  rm -f osdd
greekclobber: ; rm -f greek
delete:  clobber mandelete mcvtdelete mmdelete mmtdelete orgdelete osdddelete \
		greekdelete termhdelete texthdelete
	:
mandelete:	manclobber
	rm -f man.sh
mcvtdelete:	mcvtclobber
	rm -f mancvt.sh
mmdelete:	mmclobber
	rm -f mm.sh
mmtdelete mvtdelete:	mmtclobber mvtclobber
	rm -f mmt.sh
orgdelete:	orgclobber
	rm -f org.sh
osdddelete:	osddclobber
	rm -f osdd.sh
greekdelete:	greekclobber
	rm -f greek.sh
termhdelete: ; rm -f term
texthdelete: ; rm -f text
