LIBNAME = ../vislib2
INCRT = $(ROOT)/usr/include
CC = $(ROOT)/bin/cc
CFLAGS = -X -O -Dz8000 -DVPMICP -DVPMSYS -DVPMSYSIS -I$(INCRT)
FRC =

FILES =\
	$(LIBNAME)(bio.o)\
	$(LIBNAME)(tt0.o)\
	$(LIBNAME)(tty.o)\
	$(LIBNAME)(dk.o)\
	$(LIBNAME)(err.o)\
	$(LIBNAME)(ic.o)\
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
	$(LIBNAME)(si.o)\
	$(LIBNAME)(icpmisc.o)\
	$(LIBNAME)(sp.o)\
	$(LIBNAME)(vpmsp.o)\
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
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) dk.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) err.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) ic.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) mem.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) mt.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) pp.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) prof.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) pt.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) pd.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) is.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) pwr.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) rm.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) bio.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) sp.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) sys.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) vpmsp.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP -I$(INCRT) icpmisc.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) si.c
	cc -X -O -c -Dz8000 -DVPMSYS -DVPMSYSIS -DVPMICP  -I$(INCRT) partab.c
	cc -X -O -c -Dz8000 -DVPMSYSIS -DVPMICP  -I$(INCRT) lp.c
	cc -X -O -c -Dz8000 -DVPMSYSIS -DVPMICP  -I$(INCRT) us.c
	cc -X -O -c -Dz8000 -DVPMSYSIS -DVPMICP  -I$(INCRT) trace.c
	cc -X -O -c -Dz8000 -DVPMSYSIS -DVPMICP  -I$(INCRT) tt0.c
	cc -X -O -c -Dz8000 -DVPMSYSIS -DVPMICP  -I$(INCRT) tty.c
	ar cr $(LIBNAME) `lorder *.o | tsort` 
	ar d  $(LIBNAME) icpmisc.o
	ar rv $(LIBNAME) icpmisc.o
	strip $(LIBNAME)
	-rm -f *.o

print:
	pr -n visio.mk ../io/*.c | lpr &

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

$(LIBNAME)(is.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/io.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/is.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(lp.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h
	cc -c -X -O -Dz8000 -DVPMSYSIS -I$(INCRT) $<
	ar rv $(LIBNAME) lp.o
	rm -f lp.o
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
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/imsc.h\
	$(INCRT)/sys/iobuf.h\
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
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/imsc.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(rm.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/rm.h\
	$(INCRT)/sys/systm.h\
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
	$(INCRT)/icp/siocprom.h
	cc -c -X -O -Dz8000 -DVPMICP -DVPMSYS -DVPMSYSIS -I$(INCRT) si.c
	ar rv $(LIBNAME) si.o
	rm -f si.o
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

$(LIBNAME)(vpmsp.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/maxuser.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/tty.h\
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
	$(INCRT)/sys/user.h
	cc -c -X -O -Dz8000 -DVPMSYSIS -I$(INCRT) $<
	ar rv $(LIBNAME) trace.o
	rm -f trace.o
	$(FRC)

$(LIBNAME)(tt0.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/ioctl.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h
	cc -c -X -O -Dz8000 -DVPMSYSIS -I$(INCRT) $<
	ar rv $(LIBNAME) tt0.o
	rm -f tt0.o
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
	$(INCRT)/sys/user.h
	cc -c -X -O -Dz8000 -DVPMSYSIS -I$(INCRT) $<
	ar rv $(LIBNAME) tty.o
	rm -f tty.o
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
	$(INCRT)/sys/user.h
	cc -c -X -O -Dz8000 -DVPMSYSIS -I$(INCRT) $<
	ar rv $(LIBNAME) us.o
	rm -f us.o
	$(FRC)

FRC:
