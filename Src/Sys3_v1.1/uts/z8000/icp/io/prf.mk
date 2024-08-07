INCRT = $(ROOT)/usr/include
CFLAGS = -O -DICP -Dz8000 -DPROFILE -I$(INCRT)
FRC =

FILES =\
	ctc.o\
	dh.o\
	dma.o\
	partab.o\
	pio.o\
	tt0.o\
	tty.o

all:	$(FILES)

ctc.o:\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/dma.h\
	$(INCRT)/icp/ctc.h
	cc $(CFLAGS) -c $<
	$(FRC)

dh.o:\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/siocunix.h\
	$(INCRT)/icp/proc.h\
	$(INCRT)/icp/sioccomm.h\
	$(INCRT)/icp/icp.h\
	$(INCRT)/icp/dma.h\
	$(INCRT)/icp/ctc.h\
	$(INCRT)/icp/pio.h\
	$(INCRT)/icp/sio.h\
	$(INCRT)/icp/icpinfo.h
	cc $(CFLAGS) -c $<
	$(FRC)

dma.o:\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/dma.h\
	$(INCRT)/icp/pio.h
	cc $(CFLAGS) -c $<
	$(FRC)

partab.o:
	cc $(CFLAGS) -c $<
	$(FRC)

pio.o:\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/pio.h
	cc $(CFLAGS) -c $<
	$(FRC)

tt0.o:\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/icp/icpinfo.h
	cc $(CFLAGS) -c $<
	$(FRC)

tty.o:\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/ttold.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/ioctl.h\
	$(INCRT)/icp/sio.h\
	$(INCRT)/icp/icpinfo.h
	cc $(CFLAGS) -c $<
	$(FRC)

FRC:
