INS = :
IFLAG = -i
STRIP = -s
INSDIR = /stand
CFLAGS = -O
EXECS = mkfs od dconfig du ls cat

all:	$(EXECS) help boot dformat restor fbackup dd fsck fsdb

sashp:
	cd sash; make -f sash.mk INS=$(INS)

$(EXECS)::
	scc -c -O /usr/src/cmd/$@.c
	scc $(STRIP) $(IFLAG) -o $@ $@.o
	$(INS) -f $(INSDIR) $@

dd::
	scc -c -O /usr/src/cmd/$@.c
	scc $(STRIP) -i -o $@ $@.o
	$(INS) -f $(INSDIR) $@

fsck::
	scc -c -O /usr/src/cmd/$@.c
	scc $(STRIP) -i -o $@ $@.o
	$(INS) -f $(INSDIR) $@

fsdb::
	scc -c -O /usr/src/cmd/$@.c
	scc $(STRIP) -i -o $@ $@.o
	$(INS) -f $(INSDIR) $@

help::
	scc -c -O $@.c
	scc $(STRIP) -i -o $@ $@.o
	$(INS) -f $(INSDIR) $@

boot::
	scc -c -O $@.c
	scc $(STRIP) -o $@ $@.o
	$(INS) -f $(INSDIR) $@

dformat::
	scc -c -O xiformat.c xdformat.c
	scc $(STRIP) -i -o $@ xiformat.o xdformat.o
	$(INS) -f $(INSDIR) $@

restor::
	scc -c -O /usr/src/cmd/$@.c
	scc $(STRIP) -i -o $@ $@.o
	$(INS) -f $(INSDIR) $@

fbackup::
	scc -c -O xibackup.c xfbackup.c
	scc $(STRIP) $(IFLAG) -o $@ xibackup.o xfbackup.o
	$(INS) -f $(INSDIR) $@

install:
	make -f z8000.mk all INS=/etc/install

clean:
	rm -f *.o

clobber: clean
	rm -f $(EXECS) iltd rp6fmt fsdb
	cd boots; make -f boots.mk clobber
	cd sash; make -f sash.mk clobber

tape:
	/bin/dd if=/stand/help of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=boot of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=/sys3 of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=dformat of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=mkfs of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=restor of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=fsck of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=dd of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=fbackup of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=od of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=dconfig of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=/stand/help of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=fsdb of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=du of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=ls of=/dev/nrmt0 bs=2b conv=sync
	/bin/dd if=cat of=/dev/nrmt0 bs=2b conv=sync
