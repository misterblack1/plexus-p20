INCRT = $(ROOT)/usr/include

prf:	\
	cf/c.c\
	cf/l.s\
	cf/mch.s\
	io/ctc.c\
	io/dh.c\
	io/dma.c\
	io/partab.c\
	io/pio.c\
	io/tt0.c\
	io/tty.c\
	ml/cinit.c\
	ml/clist.s\
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
	$(INCRT)/icp/ctc.h\
	$(INCRT)/icp/dma.h\
	$(INCRT)/icp/icp.h\
	$(INCRT)/icp/icpinfo.h\
	$(INCRT)/icp/pbsioc.h\
	$(INCRT)/icp/pio.h\
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
	cd cf; make -f prf.mk "INCRT=$(INCRT)"
	cd ml; make -f prf.mk "INCRT=$(INCRT)"
	cd io; make -f prf.mk "INCRT=$(INCRT)"
	cd os; make -f prf.mk "INCRT=$(INCRT)"
	-ld -o prf -r -d -X cf/l.o cf/mch.o cf/c.o ml/c*.o os/*.o io/*.o

clean:
	-rm -f cf/*.o ml/c*.o io/*.o os/*.o

clobber: clean
	-rm -f icp

print:
	cd cf; make -f cf.mk print
	cd ml; make -f ml.mk print
	cd io; make -f io.mk print
	cd os; make -f os.mk print


profile:	unix/*.c
	cd unix; make -f profile
	-ld -o icp.o -r -d -X unix/*.o
	cp icp.o ../icp.o

install:
	cp icp $(ROOT)/usr/lib/dnld
