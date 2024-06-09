rje1:	send
	cd src; make -f src.mk install
	cd util; make -f util.mk install
	cd icp; make -f icp.mk install
	cd src; make -f rje1.mk

rje2:	rje1
	cd src; make -f rje2.mk

rje3:	rje2
	cd src; make -f rje3.mk

rje4:	rje3
	cd src; make -f rje4.mk

send:
	cd send.d; make -f send.mk install

rjestat:
	cd util; make -f util.mk rjestat

clobber:
	cd src; make -f src.mk clean
	cd send.d; make -f send.mk clean
	cd send.d/lib; make -f librje.mk clean
	cd util; make -f util.mk clean
	cd icp; make -f icp.mk clean
	cd lib; make -f lib.mk clean
