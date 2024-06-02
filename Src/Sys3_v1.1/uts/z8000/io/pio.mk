#added et.o 9/10/82 PAF
LIBNAME = ../plib2
INCRT = $(ROOT)/usr/include
CFLAGS = -X -O -I$(INCRT) -D$(HWFP) -D$(OVKRNL)
FRC =

FILES =\
	$(LIBNAME)(bio.o)\
	$(LIBNAME)(tt0.o)\
	$(LIBNAME)(tty.o)\
	$(LIBNAME)(dk.o)\
	$(LIBNAME)(err.o)\
	$(LIBNAME)(ic.o)\
	$(LIBNAME)(iotimo.o)\
	$(LIBNAME)(is.o)\
	$(LIBNAME)(lp.o)\
	$(LIBNAME)(mem.o)\
	$(LIBNAME)(mt.o)\
	$(LIBNAME)(partab.o)\
	$(LIBNAME)(pd.o)\
	$(LIBNAME)(pp.o)\
	$(LIBNAME)(prof.o)\
	$(LIBNAME)(pt.o)\
	$(LIBNAME)(pwr.o)\
	$(LIBNAME)(rm.o)\
	$(LIBNAME)(et.o)\
	$(LIBNAME)(si.o)\
	$(LIBNAME)(icpmisc.o)\
	$(LIBNAME)(sp.o)\
	$(LIBNAME)(sys.o)\
	$(LIBNAME)(trace.o)\
	$(LIBNAME)(us.o)

all:	$(LIBNAME) 

.PRECIOUS:	$(LIBNAME)

$(LIBNAME):	$(FILES)
	strip $(LIBNAME)

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME)

print:
	lnum oio.mk ../io/*.c > /dev/lp

$(LIBNAME)(bio.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(dk.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(FRC)

$(LIBNAME)(err.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/erec.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/mba.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/utsname.h\
	$(FRC)

$(LIBNAME)(ic.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/icp/siocunix.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(icpmisc.o):\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/icp/icp.h\
	$(INCRT)/icp/pbsioc.h\
	$(INCRT)/icp/sioccomm.h\
	$(INCRT)/icp/siocprom.h\
	$(FRC)

$(LIBNAME)(iotimo.o):\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/ttold.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/icp/icp.h\
	$(INCRT)/icp/pbsioc.h\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/siocunix.h\
	$(INCRT)/icp/sioccomm.h\
	$(INCRT)/icp/siocprom.h\
	$(FRC)

$(LIBNAME)(is.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/io.h\
	$(INCRT)/sys/is.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(lp.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(mem.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(mt.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(FRC)

$(LIBNAME)(partab.o):\
	$(INCRT)/sys/plexus.h\
	$(FRC)

$(LIBNAME)(pp.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/icp/siocunix.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(pwr.o):\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/systm.h\
	$(FRC)

$(LIBNAME)(pd.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/imsc.h\
	$(INCRT)/sys/disk.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/pd.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(prof.o):\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/map.h\
	$(FRC)

$(LIBNAME)(pt.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/imsc.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(rm.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/rm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(si.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/ioctl.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/ttold.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/icp/icp.h\
	$(INCRT)/icp/pbsioc.h\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/siocunix.h\
	$(INCRT)/icp/sioccomm.h\
	$(INCRT)/icp/siocprom.h\
	$(FRC)

$(LIBNAME)(sp.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/maxuser.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/icp/siocunix.h\
	$(FRC)

$(LIBNAME)(sys.o):\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(trace.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/ioctl.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(tt0.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/ioctl.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(tty.o):\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/ioctl.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/ttold.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(us.o):\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/maxuser.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/usart.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(et.o):\
	$(INCRT)/ether/state.h\
	$(INCRT)/ether/status.h\
	$(INCRT)/ether/sizes.h\
	$(INCRT)/ether/buffers.h\
	$(INCRT)/ether/command.h\
	$(INCRT)/ether/reg.h\
	$(FRC)

FRC:
