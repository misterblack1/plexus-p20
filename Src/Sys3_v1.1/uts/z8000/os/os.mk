LIBNAME = ../lib1
INCRT = $(ROOT)/usr/include
CFLAGS = -X -O -I$(INCRT) 
FRC =
FILES = \
	$(LIBNAME)(main.o)\
	$(LIBNAME)(trap.o)\
	$(LIBNAME)(sysent.o)\
	$(LIBNAME)(sys1.o)\
	$(LIBNAME)(sys2.o)\
	$(LIBNAME)(sys3.o)\
	$(LIBNAME)(sys4.o)\
	$(LIBNAME)(acct.o)\
	$(LIBNAME)(alloc.o)\
	$(LIBNAME)(clock.o)\
	$(LIBNAME)(errlog.o)\
	$(LIBNAME)(fio.o)\
	$(LIBNAME)(iget.o)\
	$(LIBNAME)(lockf.o)\
	$(LIBNAME)(machdep.o)\
	$(LIBNAME)(malloc.o)\
	$(LIBNAME)(mbm.o)\
	$(LIBNAME)(nami.o)\
	$(LIBNAME)(pipe.o)\
	$(LIBNAME)(prf.o)\
	$(LIBNAME)(rdwri.o)\
	$(LIBNAME)(sig.o)\
	$(LIBNAME)(slp.o)\
	$(LIBNAME)(subr.o)\
	$(LIBNAME)(sysdebug.o)\
	$(LIBNAME)(text.o)\
	$(LIBNAME)(tod.o)\
	$(LIBNAME)(ureg.o)\
	$(LIBNAME)(utssys.o)


all:	$(LIBNAME) 

.PRECIOUS:	$(LIBNAME)

$(LIBNAME):	$(FILES)
	strip $(LIBNAME)

clean:
	rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME)

print:
	lnum os.mk ../os/*.c > /dev/lp

$(LIBNAME)(acct.o):\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(alloc.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/fblk.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/ino.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(clock.o):\
	$(INCRT)/sys/callo.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/psl.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(errlog.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/erec.h\
	$(INCRT)/sys/err.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/utsname.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(fio.o):\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(iget.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/ino.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(lockf.o.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(machdep.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/psl.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/utsname.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(main.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(malloc.o):\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/systm.h\
	$(FRC)

$(LIBNAME)(mbm.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(nami.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(pipe.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(prf.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/elog.h\
	$(INCRT)/sys/iobuf.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/utsname.h\
	$(FRC)

$(LIBNAME)(rdwri.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(sig.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/psl.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(slp.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(subr.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(sys1.o):\
	$(INCRT)/sys/acct.h\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(sys2.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(sys3.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/ino.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/ioctl.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/stat.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(sys4.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(sysdebug.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/usart.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/trap.h\
	$(FRC)

$(LIBNAME)(sysent.o):\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/systm.h\
	$(FRC)

$(LIBNAME)(text.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/inode.h\
	$(INCRT)/sys/map.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/var.h\
	$(FRC)

$(LIBNAME)(tod.o):\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(FRC)

$(LIBNAME)(trap.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/psl.h\
	$(INCRT)/sys/reg.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/sysinfo.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/trap.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/utsname.h\
	$(FRC)

$(LIBNAME)(ureg.o):\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/plexus.h\
	$(INCRT)/sys/proc.h\
	$(INCRT)/sys/seg.h\
	$(INCRT)/sys/systm.h\
	$(INCRT)/sys/text.h\
	$(INCRT)/sys/user.h\
	$(FRC)

$(LIBNAME)(utssys.o):\
	$(INCRT)/sys/buf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/filsys.h\
	$(INCRT)/sys/mount.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/user.h\
	$(INCRT)/sys/utsname.h\
	$(INCRT)/sys/var.h\
	$(FRC)

FRC:
