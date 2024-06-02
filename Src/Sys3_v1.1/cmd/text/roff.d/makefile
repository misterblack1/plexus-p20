#	Development make file for nroff/troff
#	SCCS:  @(#)makefile	1.20
#
# Regular make options:
#	nroff	make nroff
#	troff	make troff ('make PTAG= troff')
#	suftab	make suftab
#	lint	lint the nroff/troff source code
#	clean	remove temporary object files
#	clobber	remove all object files
#	delete	remove all source files
#	help	print this menu
#
# Development make options:
#	dnroff	make debug version of nroff (dnroff)
#	dtroff	make debug version of troff (dtroff)
#	edit	prepare [nt]roff source for editing
#	delta	perform a delta on edited source
#	bldtab	generate name table builder program
#	listing	generate a listing (to bin ${BIN})


OL = /usr
SL = /usr/src/cmd
RDIR = ${SL}/text/roff.d
INS = :
REL = current
CSID = -r`gsid nroff ${REL}`
BSID = -r`gsid bldtab ${REL}`
MAKSID = -r`gsid rmakefile ${REL}`
MKSID = -r`gsid roff.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}/bin
DINSDIR = ${OL}/lib
STRIP = 

PTAG = -DNROFF
IFLAG = -i
CFLAGS = -O $(PTAG) $(OPTS) ${FFLAG}
NTSOURCE = hytab.c n1.c n10.c n[2-9i].c nii.c ntab.c strs.hd t10.c \
	 t6.c tab3.c tdef.hd tw.hd uns.hd def.nr.names def.rq.names
SMSOURCE = suftabmake.c suftab.c
SOURCE = ${NTSOURCE} ${SMSOURCE}
NFILES = n1.o n2.o n3.o n4.o n5.o n6.o n7.o n8.o n9.o n10.o ni.o \
		nii.o ntab.o hytab.o
TFILES = n1.o n2.o n3.o n4.o n5.o t6.o n7.o n8.o n9.o t10.o ni.o \
		nii.o tab3.o hytab.o

nroff:	$(NFILES)
	${CC} $(IFLAG) $(OPTS) ${FFLAG} ${STRIP} $(LDFLAGS) -o nroff $(NFILES)
	chmod 755 nroff

troff:	checkPTAG $(TFILES)
	${CC} $(IFLAG) $(OPTS) ${FFLAG} ${STRIP} $(LDFLAGS)  -o troff $(TFILES)
	chmod 755 troff

checkPTAG:	;  -(if [ -n "$(PTAG)" ];\
			then (echo "must use PTAG="; exit 1);\
			else exit 0; fi)

n[18].o::	tdef.hd strs.hd tw.hd uns.hd
	:
n[2-79].o n10.o nii.o::	tdef.hd strs.hd tw.hd
	:
t6.o t10.o::	tdef.hd strs.hd
	:
ni.o::	tdef.hd nrtab.dt rqtab.dt
	:

#	***** NOTE *****  the 300 in the next line is hardwired for (NN)
nrtab.dt: bldtab def.nr.names
	./bldtab 300 def.nr.names > nrtab.dt

#	***** NOTE *****  the 350 in the next line is hardwired for (NM)
rqtab.dt:  bldtab def.rq.names
	./bldtab 350 def.rq.names > rqtab.dt

suftab:	suftabmake.c suftab.c
	${CC} suftabmake.c
	./a.out
	chmod 644 suftab

bldtab:	bldtab.c tdef.hd
	cc -s -O -o bldtab bldtab.c

build:	bldmk
	get -p ${CSID} s.roff.src ${REWIRE} | ntar -d ${RDIR} -g
	get -p ${BSID} s.bldtab.c > ${RDIR}/bldtab.c
bldmk: ; get -p ${MAKSID} s.makefile > ${RDIR}/makefile

edit:	ntedit bedit makeedit
	:
delta:	ntdelta bdelta makedelta
	:

ntedit:	;  get -e -p s.roff.src | ntar -g
ntdelta:  ;  ntar -p ${SOURCE} > roff.src
	delta s.roff.src
	rm -f ${SOURCE}

bedit:	;  get -e s.bldtab.c
bdelta:	;  delta s.bldtab.c

makeedit: ;  get -e s.makefile
makedelta: ; delta s.makefile

listing:	n1.c bldtab.c
	pr -r README makefile ${SOURCE} bldtab.c | ${LIST}
	cref -c ${NTSOURCE} | pr | ${LIST}
	@echo "listings and crefs to bin ${BIN}"
listmk: ; pr makefile | ${LIST}

dnroff:	$(NFILES)
	${CC} $(IFLAG) $(OPTS) ${FFLAG} -o dnroff $(NFILES)
dtroff:	checkPTAG $(TFILES)
	${CC} $(IFLAG) $(OPTS) ${FFLAG} -o dtroff $(TFILES)

lint:	n1.c
	@lint -DNROFF -haxcs n[1-9].c n10.c ni.c nii.c ntab.c hytab.c |\
			pr -h "lint of nroff"
	@lint -haxcs n[1-5].c t6.c n[7-9].c t10.c ni.c nii.c tab3.c hytab.c |\
			pr -h "lint of troff"

all install:
	@echo "wrong make file - use roff.mk";  exit 1

clean:
	rm -f *.o a.out

clobber:	clean bclobber
	rm -f nroff troff suftab dnroff dtroff
bclobber:
	rm -f bldtab ??tab.dt

delete:	clobber bdelete
	rm -f ${SOURCE}
bdelete: ;  rm -f bldtab.c
