INCRT = $(ROOT)/usr/include
CFLAGS = -O -DICP -DSMALL -DVPMSYS -Dz8000 -DDEBUG -I$(INCRT)
FRC =

FILES =\
	siocdebug.o

all:	$(FILES)

siocdebug.o:\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/icp/proc.h\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/sio.h\
	$(INCRT)/icp/sioccomm.h\
	$(INCRT)/icp/icp.h\
	$(INCRT)/icp/pbsioc.h
	/bin/cc.old $(CFLAGS) -c $<
	$(FRC)

FRC:
