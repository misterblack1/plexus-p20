all:
	cd z8000;\
	make -f dz8000.mk "INS=$(INS)" "INSDIR=$(INSDIR)" "INCRT=$(INCRT)"\
		"FRC=$(FRC)" "SYS=$(SYS)" "NODE=$(NODE)" "REL=$(REL)"\
		"VER=$(VER)" "TYPE=$(TYPE)" "ROOT=$(ROOT)" "USERS=$(USERS)"\
		"HWFP=$(HWFP)";

clean:
	cd z8000; make -f z8000.mk clean ;

clobber:
	cd z8000; make -f z8000.mk clobber "SYS=$(SYS)" "VER=$(VER)" ;
