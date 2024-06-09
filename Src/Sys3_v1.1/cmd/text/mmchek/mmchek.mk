#	mmchek make file
#	SCCS:	@(#)mmchek.mk	1.3

OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/mmchek
INS = :
REL = current
CSID = -r`gsid mmchek ${REL}`
MKSID = -r`gsid mmchek.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
INSDIR = ${OL}/bin
CFLAGS = -O ${FFLAG}
IFLAG = -n
SOURCE = chekl.l chekmain.c
FILES = chekl.o chekmain.o
MAKE = make

compile all: mmchek
	:

mmchek:	$(FILES)
	$(CC) $(LDFLAGS) ${IFLAG} -o mmchek $(FILES) -ll -lPW
	$(INS) mmchek $(INSDIR)
	chmod 755 ${INSDIR}/mmchek
	-if [ "${OL}" = "/usr" ]; \
		then cd ${INSDIR}; chown bin mmchek; chgrp bin mmchek; \
	 fi

$(FILES)::
	:

install:
	${MAKE} -f mmchek.mk INS=cp OL=${OL} SL=${SL} LDFLAGS="$(LDFLAGS)"

build:	bldmk
	get -p ${CSID} s.mmchek.src ${REWIRE} | ntar -d ${RDIR} -g
bldmk:  ;  get -p ${MKSID} s.mmchek.mk > ${RDIR}/mmchek.mk

listing:
	pr mmchek.mk ${SOURCE} | ${LIST}
listmk: ;  pr mmchek.mk | ${LIST}

edit:
	get -e -p s.mmchek.src | ntar -g

delta:
	ntar -p ${SOURCE} > mmchek.src
	delta s.mmchek.src
	rm -f ${SOURCE}

mkedit:  ;  get -e s.mmchek.mk
mkdelta: ;  delta s.mmchek.mk

clean:
	  rm -f ${FILES}

clobber:  clean
	  rm -f mmchek

delete:	clobber
	rm -f ${SOURCE}
