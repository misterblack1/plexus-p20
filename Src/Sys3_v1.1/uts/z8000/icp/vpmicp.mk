INCRT = $(ROOT)/usr/include

vpm0:\
	cf/c.c\
	cf/l.s\
	cf/mch.s\
	io/ctc.c\
	io/dh.c\
	io/dma.c\
	io/partab.c\
	io/tt0.c\
	io/tty.c\
	ml/vpmclist.c\
	os/bcopy.s\
	os/blist.c\
	os/clock.c\
	os/daemon.c\
	os/main.c\
	os/mb.c\
	os/pbif.c\
	os/pbs.c\
	os/prf.c\
	os/sched.c\
	os/slp.c\
	os/subr.c\
	os/trap.c\
	vpmicp/get.c\
	vpmicp/interp.c\
        vpmicp/xmt.c\
        vpmicp/xeom.c\
	vpmicp/getxbuf.c\
	vpmicp/rtnxbuf.c\
	vpmicp/rcv.c\
	vpmicp/put.c\
	vpmicp/getrbuf.c\
	vpmicp/rtnrbuf.c\
	vpmicp/proto0.s\
	vpmicp/proto1.s\
	vpmicp/proto2.s\
	vpmicp/proto3.s\
	vpmicp/fetch.c\
	$(INCRT)/icp/ctc.h\
	$(INCRT)/icp/dma.h\
	$(INCRT)/icp/icp.h\
	$(INCRT)/icp/icpinfo.h\
	$(INCRT)/icp/pbsioc.h\
	$(INCRT)/icp/proc.h\
	$(INCRT)/icp/sio.h\
	$(INCRT)/icp/sioc.h\
	$(INCRT)/icp/sioccomm.h\
	$(INCRT)/icp/sioccsv.h\
	$(INCRT)/icp/siocprom.h\
	$(INCRT)/icp/siocunix.h\
	$(INCRT)/sys/callo.h\
	$(INCRT)/sys/conf.h\
	$(INCRT)/sys/dir.h\
	$(INCRT)/sys/file.h\
	$(INCRT)/sys/ioctl.h\
	$(INCRT)/sys/param.h\
	$(INCRT)/sys/ttold.h\
	$(INCRT)/sys/tty.h\
	$(INCRT)/sys/user.h
	cd cf; make -f vpmcf.mk "INCRT=$(INCRT)"
	cd ml; make -f vpmml.mk "INCRT=$(INCRT)"
	cd io; make -f vpmio.mk "INCRT=$(INCRT)"
	cd os; make -f vpmos.mk "INCRT=$(INCRT)"
	cd vpmicp; make -f vpmicpos.mk "INCRT=$(INCRT)"
	-ld -o vpm0 -r -d -X cf/l.o cf/mch.o cf/c.o ml/vpm*.o os/*.o io/*.o\
		vpmicp/*.o

clean:
	-rm -f cf/*.o ml/vpm*.o io/*.o os/*.o vpmicp/*.o

clobber: clean
	-rm -f vpm0

print:
	cd cf; make -f vpmcf.mk print
	cd ml; make -f vpmml.mk print
	cd io; make -f vpmio.mk print
	cd os; make -f vpmos.mk print
	cd vpmicp; make -f vpmicpos.mk print


profile:	unix/*.c
	cd unix; make -f profile
	-ld -o icp.o -r -d -X unix/*.o
	cp icp.o ../icp.o
