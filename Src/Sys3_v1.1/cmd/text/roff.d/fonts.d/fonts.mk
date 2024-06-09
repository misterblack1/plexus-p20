#	troff font tables make file
#	SCCS:  @(#)fonts.mk	1.10

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/roff.d/fonts.d
REL = current
FSID = -r`gsid fonts ${REL}`
CSID = -r`gsid makefonts ${REL}`
MKFSID = -r`gsid mkfont ${REL}`
MKSID = -r`gsid fonts.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
INSDIR = ${OL}/lib/font
FFILES = chars.c ftB.c ftBC.c ftC.c ftCE.c ftCI.c ftCK.c ftCS.c ftCW.c \
	 ftG.c ftGI.c ftGM.c ftGR.c ftI.c ftL.c ftLI.c ftPA.c ftPB.c \
	 ftPI.c ftR.c ftS.c ftSB.c ftSI.c ftSM.c ftUD.c ftXM.c
PFILES = makefonts.c
MKFILES = mkfont.c mkfont1.c
FILES = ftB ftBC ftC ftCE ftCI ftCK ftCS ftCW ftG ftGI ftGM ftGR ftI \
	ftL ftLI ftPA ftPB ftPI ftR ftS ftSB ftSI ftSM ftUD ftXM


makefonts compile all:
	${CC} -o makefonts makefonts.c
	./makefonts
	chmod 644 ${FILES}

install:  makefonts mkfont
	/etc/install -n $(INSDIR) ./mkfont $(INSDIR)
	cp ${FILES} ${INSDIR}
	cd ${INSDIR}; rm -f ftH ftHI ftHM ftFD ftCI
	cd ${INSDIR}; ln ftG ftH; ln ftGI ftHI; ln ftGM ftHM; \
		ln ftUD ftFD; ln ftCK ftCI
	-if [ "${OL}" = "/usr" ]; \
		then cd ${INSDIR}; chown bin ${FILES}; chgrp bin ${FILES}; \
	 fi

mkfont:  mkfont.c mkfont1.c 
	${CC} -n -s mkfont.c -o mkfont

build:  bldmk bldf bldmakf bldmf
	:
bldf:	;  get -p ${FSID} s.fonts.src | ntar -d ${RDIR} -g
bldmakf: ; get -p ${CSID} s.makefonts.c > ${RDIR}/makefonts.c
bldmf:	;  get -p ${MKFSID} s.mkfont.src | ntar -d ${RDIR} -g
bldmk: ; get -p ${MKSID} s.fonts.mk > ${RDIR}/fonts.mk

listing:
	pr fonts.mk ${PFILES} ${FFILES} ${MKFILES} | ${LIST}
listmk: ;  pr fonts.mk | ${LIST}
listf:	;  pr ${FFILES} | ${LIST}
listmf:	;  pr ${MKFILES} | ${LIST}
listmakf: ; pr ${PFILES} | ${LIST}

edit:	mkedit fedit mfedit makfedit
	:
mkedit:	;  get -e s.fonts.mk
fedit:	;  get -e -p s.fonts.src | ntar -g
mfedit:	;  get -e -p s.mkfont.src | ntar -g
makfedit: ; get -e s.makefonts.c

delta:	mkdelta fdelta mfdelta makfdelta
	:
mkdelta:  ;  delta s.fonts.mk
fdelta:   ;  ntar -p ${FFILES} > fonts.src
	     delta s.fonts.src
	     rm -f ${FFILES}
mfdelta:  ;  ntar -p ${MKFILES} > mkfont.src
	     delta s.mkfont.src
	     rm -f ${MKFILES}
makfdelta: ; delta s.makefonts.c

clean:
	rm -f makefonts a.out
clobber:  clean
	rm -f ${FILES}
delete:	clobber
	rm -f ${FFILES} ${PFILES} ${MKFILES}
