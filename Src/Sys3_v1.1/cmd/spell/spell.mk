#	spell make file
#	SCCS:  @(#)spell.mk	1.14

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/spell
INS = :
REL = current
CSID = -r`gsid spellcode ${REL}`
DSID = -r`gsid spelldata ${REL}`
SHSID = -r`gsid spell.sh ${REL}`
CMPRSID = -r`gsid compress.sh ${REL}`
MKSID = -r`gsid spell.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}/bin
PINSDIR = ${OL}/lib/spell
IFLAG = -i
SMFLAG =
CFLAGS = -O
SFILES = spell.h spellprog.c spellin.c spellout.c
DFILES = american british local list stop
MAKE = make

compile all: spell hlista hlistb hstop spellin spellout spellprog compress
	:
alldata:  hlista hlistb hstop
	:

spell:	spellprog spell.sh
	cp spell.sh spell
	$(INS) spell $(CINSDIR)
	chmod 755 ${CINSDIR}/spell
	-if [ "${OL}" = "/usr" ]; \
		then cd ${CINSDIR}; chown bin spell; chgrp bin spell; \
	   fi

spelldir:
	-mkdir ${PINSDIR}

compress:  spelldir compress.sh
	cp compress.sh compress
	${INS} compress ${PINSDIR}
	chmod 755 ${PINSDIR}/compress
	-if [ "${OL}" = "/usr" ]; \
		then cd ${PINSDIR}; chown bin compress; chgrp bin compress; \
	 fi

spellprog: spelldir spellprog.c spell.h
	$(CC) $(LDFLAGS) $(IFLAG) ${SMFLAG} ${FFLAG} -O spellprog.c -o spellprog
	$(INS) spellprog $(PINSDIR)
	chmod 755 ${PINSDIR}/spellprog
	-if [ "${OL}" = "/usr" ]; \
		then cd ${PINSDIR}; chown bin spellprog; chgrp bin spellprog; \
	 fi

spellin: spelldir spellin.c spell.h
	$(CC) $(LDFLAGS) $(IFLAG) ${SMFLAG} ${FFLAG} -O spellin.c -o spellin
	${INS} spellin ${PINSDIR}
	chmod 755 ${PINSDIR}/spellin
	-if [ "${OL}" = "/usr" ]; \
		then cd ${PINSDIR}; chown bin spellin; chgrp bin spellin; \
	 fi

spellout: spelldir spellout.c spell.h
	$(CC) $(LDFLAGS) $(IFLAG) ${SMFLAG} ${FFLAG} -O spellout.c -o spellout
	${INS} spellout ${PINSDIR}
	chmod 755 ${PINSDIR}/spellout
	-if [ "${OL}" = "/usr" ]; \
		then cd ${PINSDIR}; chown bin spellout; chgrp bin spellout; \
	 fi


hlist:	list spellin
	./spellin < list > hlist

hlista: spelldir american local hlist
	cat american local | ./spellin hlist > hlista
	$(INS) hlista $(PINSDIR)
	chmod 644 ${PINSDIR}/hlista
	-if [ "${OL}" = "/usr" ]; \
		then cd ${PINSDIR}; chown bin hlista; chgrp bin hlista; \
	 fi

hlistb: spelldir british local hlist
	cat british local | ./spellin hlist > hlistb
	$(INS) hlistb $(PINSDIR)
	chmod 644 ${PINSDIR}/hlistb
	-if [ "${OL}" = "/usr" ]; \
		then cd ${PINSDIR}; chown bin hlistb; chgrp bin hlistb; \
	 fi

hstop:	spelldir stop spellin
	./spellin < stop > hstop
	$(INS) hstop $(PINSDIR)
	chmod 644 ${PINSDIR}/hstop
	-if [ "${OL}" = "/usr" ]; \
		then cd ${PINSDIR}; chown bin hstop; chgrp bin hstop; \
	 fi

install:  ;  ${MAKE} -f spell.mk INS=cp OL=${OL} all LDFLAGS="$(LDFLAGS)"
inssh:    ;  ${MAKE} -f spell.mk INS=cp OL=${OL} spell LDFLAGS="$(LDFLAGS)"
inscomp:  ;  ${MAKE} -f spell.mk INS=cp OL=${OL} compress LDFLAGS="$(LDFLAGS)"
inscode:  ;  ${MAKE} -f spell.mk INS=cp spell OL=${OL} LDFLAGS="$(LDFLAGS)"
insdata:  ;  ${MAKE} -f spell.mk INS=cp alldata OL=${OL} LDFLAGS="$(LDFLAGS)"

listing:  ;  pr spell.mk spell.sh compress.sh ${SFILES} ${DFILES} | ${LIST}
listmk:   ;  pr spell.mk | ${LIST}
listsh:	  ;  pr spell.sh | ${LIST}
listcomp: ;  pr compress.sh | ${LIST}
listcode: ;  pr ${SFILES} | ${LIST}
listdata: ;  pr ${DFILES} | ${LIST}

build:  bldmk bldsh bldcomp bldcode blddata
	:
bldcode:  ;  get -p ${CSID} s.spell.src ${REWIRE} | ntar -d ${RDIR} -g
blddata:  ;  get -p ${DSID} s.spell.data | ntar -d ${RDIR} -g
bldsh:	  ;  get -p ${SHSID} s.spell.sh ${REWIRE} > ${RDIR}/spell.sh
bldcomp:  ;  get -p ${CMPRSID} s.compress.sh ${REWIRE} > ${RDIR}/compress.sh
bldmk:    ;  get -p ${MKSID} s.spell.mk > ${RDIR}/spell.mk

edit:	sedit dedit mkedit shedit compedit
	:
sedit:	;  get -p -e s.spell.src | ntar -g
dedit:	;  get -p -e s.spell.data | ntar -g
shedit:	;  get -e s.spell.sh
compedit: ; get -e s.compress.sh

delta:	sdelta ddelta mkdelta shdelta compdelta
	:
sdelta:
	ntar -p ${SFILES} > spell.src
	delta s.spell.src
	rm -f ${SFILES}
ddelta:
	ntar -p ${DFILES} > spell.data
	delta s.spell.data
	rm -f ${DFILES}
shdelta:
	delta s.spell.sh
compdelta: ; delta s.compress.sh

mkedit:	;  get -e s.spell.mk
mkdelta: ; delta s.spell.mk

clean:
	rm -f *.o

clobber: clean shclobber compclobber
	rm -f spell spellprog spellin spellout hlist* hstop
shclobber: ; rm -f spell
compclobber: ; rm -f compress

delete:	clobber shdelete compdelete
	rm -f ${SFILES} ${DFILES}
shdelete: shclobber
	rm -f spell.sh
compdelete: compclobber
	rm -f compress.sh
