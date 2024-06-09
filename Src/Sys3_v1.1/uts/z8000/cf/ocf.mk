 
SYS = sys3
NODE = sys3
REL = 3.0.1
VER = isrm

ROOT =
INS = /etc/install
INSDIR = /usr/src/test
INCRT = $(ROOT)/usr/include
CFLAGS = -X -O -B/lib/k -I$(INCRT) -D$(HWFP) -D$(OVKRNL) -D$(PNETDFS) 
FRC =
NAME = $(SYS)$(VER)

all:	init ../$(NAME)

init:
	-if cmp $(INCRT)/sys/maxuser.h\
			$(INCRT)/sys/maxuser$(USERS).h > /dev/null;\
		then echo "	USERS =" $(USERS);\
		else cp $(INCRT)/sys/maxuser$(USERS).h\
			$(INCRT)/sys/maxuser.h; echo "	USERS =" $(USERS);\
	fi;
	cd $(ROOT)/usr/src/uts; make -f outs.mk "INS=$(INS)" \
		"INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" "USERS=$(USERS)" \
		"HWFP=$(HWFP)" "OVKRNL=$(OVKRNL)" "PNETDFS=$(PNETDFS)"
	-@$(CC) $(CFLAGS) -c -DSYS=\"`expr '$(SYS)' : '\(.\{1,8\}\)'`\" \
		-DNODE=\"`expr '$(NODE)' : '\(.\{1,8\}\)'`\" \
		-DREL=\"`expr '$(REL)' : '\(.\{1,8\}\)'`\" \
		-DVER=\"`expr '$(VER)' : '\(.\{1,8\}\)'`\" name.c

../$(NAME): low.o conf.o ../locore.o ../lib[0-9] linesw.o name.o
	mv ../olib1 ../solib1
	cp ../solib1 ../olib1
	ar x ../olib1 sysdebug.o clock.o trap.o main.o machdep.o \
		rdwri.o slp.o \
		sys1.o sys2.o nami.o fio.o iget.o
	ar d ../olib1 sysdebug.o clock.o trap.o main.o machdep.o \
		rdwri.o slp.o \
		sys1.o sys2.o nami.o fio.o iget.o
	mv ../olib2 ../solib2
	cp ../solib2 ../olib2
	ar x ../olib2 us.o iotimo.o 
	ar d ../olib2 us.o iotimo.o 
	/bin/ovld -X -i low.o ../locore.o conf.o \
		sysdebug.o clock.o trap.o main.o machdep.o \
		sys1.o sys2.o nami.o \
		slp.o \
		us.o iotimo.o  \
		linesw.o ../olib0 name.o \
		-Z ../olib1  -Z  iget.o fio.o rdwri.o ../olib2 -L \
		-o ../$(NAME);
	rm sysdebug.o clock.o trap.o main.o machdep.o rdwri.o \
		slp.o sys1.o sys2.o nami.o fio.o iget.o
	rm us.o iotimo.o 
	mv ../solib1 ../olib1
	mv ../solib2 ../olib2

clean:
	cd $(ROOT)/usr/src/uts; make -f uts.mk "INS=$(INS)" "INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" clean
	-rm -f *.o

clobber:	clean
	cd $(ROOT)/usr/src/uts; make -f uts.mk "INS=$(INS)" "INSDIR=$(INSDIR)" \
		"INCRT=$(INCRT)" "FRC=$(FRC)" "SYS=$(SYS)" \
		"NODE=$(NODE)" "REL=$(REL)" "VER=$(VER)" clobber
	-rm -f *.o

print:
	lnum cf.mk ../cf/*.s ../cf/*.c > /dev/lp

conf.c:\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/callo.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/err.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/io.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/peri.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/space.h\
	$(INCRT)/icp/siocunix.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/var.h\
	$(FRC)

linesw.c:\
	$(INCRT)/sys/conf.h\
	$(FRC)

low.s:\
	$(INCRT)/sys/plexus.h\
	$(FRC)

name.c:\
	$(INCRT)/sys/utsname.h\
	$(FRC)

install:	all
	$(INS) -o -f $(INSDIR) "../$(NAME)"

.s.o:
	/lib/cpp -I$(INCRT) -P  $< >tempfile
	as -u tempfile -o $@
	rm tempfile

FRC:
