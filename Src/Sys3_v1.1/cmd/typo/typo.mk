#	typo make file - supports text development support library
#	SCCS:  @(#)typo.mk	1.8

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/typo
INS = cp
REL = current
CSID = -r`gsid typoprog ${REL}`
DSID = -r`gsid typodata ${REL}`
TYPOSID = -r`gsid typo.sh ${REL}`
MKSID = -r`gsid typo.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}/bin
INSDIR = ${OL}/lib
CFILES = pinhead.h typoprog.c
DFILES = salt sq2006

compile all:	pinhead.h typoprog.c typosh
	${CC} $(LDFLAGS) ${FFLAG} -O -o typoprog typoprog.c -lm
typosh: ; cp typo.sh typo
	  chmod 755 typo

install:  inscode insdata instypo
	  :
inscode:  all
	  cp typoprog ${INSDIR}
	  chmod 755 ${INSDIR}/typoprog
	  -if [ "${OL}" = "/usr" ]; \
		then cd ${INSDIR}; chown bin typoprog; chgrp bin typoprog; \
	   fi
instypo:  typosh
	  cp typo ${CINSDIR}
	  chmod 755 ${CINSDIR}/typo
	  -if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin typo; chgrp bin typo; \
	   fi
insdata:
	  cp salt sq2006 ${INSDIR}
	  chmod 644 ${INSDIR}/salt ${INSDIR}/sq2006
	  -if [ "${OL}" = "/usr" ]; \
		then cd ${INSDIR}; chown bin salt sq2006; chgrp bin salt sq2006; \
	   fi

listing:  ;  pr typo.mk typo.sh ${CFILES} | ${LIST}
listmk:    ;  pr typo.mk | ${LIST}
listtypo:  ;  pr typo.sh | ${LIST}
listcode:  ;  pr ${CFILES} | ${LIST}

build:  bldmk bldcode blddata bldtypo
	  :
bldmk:    ;  get -p ${MKSID} s.typo.mk > ${RDIR}/typo.mk
bldcode:  ;  get -p ${CSID} s.typoprog.src ${REWIRE} | ntar -d ${RDIR} -g
blddata:  ;  cp ${DFILES} ${RDIR}
bldtypo:  ;  get -p ${TYPOSID} s.typo.sh > ${RDIR}/typo.sh

edit:	mkedit cedit typoedit
	:
mkedit: ;  get -e s.typo.mk
cedit:	;  get -e -p s.typoprog.src | ntar -g
typoedit: ; get -e s.typo.sh

delta:  mkdelta cdelta typodelta
	:
mkdelta: ; delta s.typo.mk
cdelta:  ;  ntar -p ${CFILES} > typoprog.src
	    delta s.typoprog.src
	    rm -f ${CFILES}
typodelta: ; delta s.typo.sh

clean cclean:
	rm -f *.o
clobber:  clean cclobber typoclobber
	:
cclobber: ; rm -f typoprog
typoclobber: ; rm -f typo
delete:   clobber cdelete ddelete typodelete
	:
cdelete: ; rm -f ${CFILES}
ddelete: ; rm -f ${DFILES}
typodelete: ; rm -f typo.sh
