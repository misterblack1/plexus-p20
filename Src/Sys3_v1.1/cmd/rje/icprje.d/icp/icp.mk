VPMC = vpmc

ARGS = -m -x

install:	rjeproto

rjeproto:	rjeproto.r
	$(VPMC) $(ARGS) -o rjeproto rjeproto.r
	fgrep define sas_tempc >sas_define
	cat /usr/include/icp/opdef.h sas_tempc | /lib/cpp >tf
	/usr/lib/vpm/vratfor <tf >tg
	cp tg /usr/src/uts/z8000/icp/vpmicp/proto0code.s
	cat sas_define proto0.s >th
	cp th /usr/src/uts/z8000/icp/vpmicp/proto0.s
	touch /usr/src/uts/z8000/icp/vpmicp/proto1.s
	touch /usr/src/uts/z8000/icp/vpmicp/proto2.s
	touch /usr/src/uts/z8000/icp/vpmicp/proto3.s
	cp proto1code.s /usr/src/uts/z8000/icp/vpmicp
	cp proto2code.s /usr/src/uts/z8000/icp/vpmicp
	cp proto3code.s /usr/src/uts/z8000/icp/vpmicp
	cd /usr/src/uts/z8000/icp;\
	make -f vpmicp.mk clean
	cd /usr/src/uts/z8000/icp;\
	make -f vpmicp.mk vpm0
	cd /usr/src/uts/z8000/icp;\
	make -f vpmicp.mk clean
	cd /usr/src/cmd/rje/icprje.d/icp;\
	cp proto0.s /usr/src/uts/z8000/icp/vpmicp
	cp /usr/src/uts/z8000/icp/vpm0 rjeproto
	cp rjeproto /etc/rjeproto
	-chmod 644 /etc/rjeproto
	-chown rje /etc/rjeproto

clean:
	-rm -f rjeproto
	-rm -f tf tg th sas*
