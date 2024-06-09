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
