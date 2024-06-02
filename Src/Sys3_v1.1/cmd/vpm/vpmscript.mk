VPMC = vpmc

ARGS = -m -x

install:	vpmscript

vpmscript:	vpmscript.r
	$(VPMC) $(ARGS) -o vpmscript vpmscript.r
	fgrep define sas_tempc >sas_define
	cat /usr/include/icp/opdef.h sas_tempc | /lib/cpp >tf
	/usr/lib/vpm/vratfor <tf >tg
	cp tg /usr/src/uts/z8000/icp/vpmicp/proto0code.s
	cat sas_define proto0.s >th
	cp th /usr/src/uts/z8000/icp/vpmicp/proto0.s
	touch /usr/src/uts/z8000/icp/vpmicp/proto1.s
	touch /usr/src/uts/z8000/icp/vpmicp/proto2.s
	touch /usr/src/uts/z8000/icp/vpmicp/proto3.s
	cd /usr/src/uts/z8000/icp/vpmicp;\
	make -f vpmicpos.mk proto0.o
	cd /usr/src/uts/z8000/icp/vpmicp;\
	make -f vpmicpos.mk proto1.o
	cd /usr/src/uts/z8000/icp/vpmicp;\
	make -f vpmicpos.mk proto2.o
	cd /usr/src/uts/z8000/icp/vpmicp;\
	make -f vpmicpos.mk proto3.o
	cd /usr/src/cmd/vpm
	-ld -o vpm0 -r -d -X \
		/usr/src/uts/z8000/icp/cf/l.o \
		/usr/src/uts/z8000/icp/cf/mch.o \
		/usr/src/uts/z8000/icp/cf/c.o \
		/usr/src/uts/z8000/icp/ml/vpm*.o \
		/usr/src/uts/z8000/icp/os/*.o \
		/usr/src/uts/z8000/icp/io/*.o \
		/usr/src/uts/z8000/icp/vpmicp/*.o


clobber: clean
	-rm -f vpm0
clean:
	-rm -f vpmscript
	-rm -f tf tg th sas*
