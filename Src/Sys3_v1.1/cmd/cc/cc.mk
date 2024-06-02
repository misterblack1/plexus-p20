TESTDIR = .
FRC =
INS = /etc/install
INSDIR = /bin
CFLAGS = -O
IFLAG = -i
LDFLAGS = -n -s
FFLAG =
YACCRM=-rm

all: cpp pcc cc c2

cc: cc.sh
	cp cc.sh cc
c2:
	cd c2.d; make cp
cpp:
	-if vax ; then cd vax; make -f cc.mk CFLAGS="$(CFLAGS)" \
	LDFLAGS="$(LDFLAGS)" TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) all ; \
	else \
		cd cpp.d; make -f cpp.mk CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" IFLAG=$(IFLAG) FFLAG=$(FFLAG) \
		TESTDIR=$(TESTDIR) all ; \
	fi
pcc:
	-if vax ; then cd vax; make -f cc.mk CFLAGS="$(CFLAGS)" \
	LDFLAGS="$(LDFLAGS)" TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) all ; \
	else \
		cd pcc.d; make -f pcc.mk CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" IFLAG=$(IFLAG) FFLAG=$(FFLAG) \
		TESTDIR=$(TESTDIR) all ; \
	fi
install: all
	-if vax ; then cd vax; make -f cc.mk CFLAGS="$(CFLAGS)" \
	LDFLAGS="$(LDFLAGS)" TESTDIR=$(TESTDIR) FFLAG=$(FFLAG) install ; \
	else \
		$(INS) -n $(INSDIR) cc ; \
		rm -f /usr/bin/pcc; \
		ln $(INSDIR)/cc /usr/bin/pcc ; \
		cd cpp.d; make -f cpp.mk CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" IFLAG=$(IFLAG) FFLAG=$(FFLAG) \
		TESTDIR=$(TESTDIR) install ; \
		cd ../pcc.d; make -f pcc.mk CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" IFLAG=$(IFLAG) FFLAG=$(FFLAG) \
		TESTDIR=$(TESTDIR) install ; \
	fi
clean:
	-rm -f cc
	-if vax  ; then cd vax; make -f cc.mk clean TESTDIR=$(TESTDIR) \
	YACCRM=$(YACCRM) ; \
	else \
		cd cpp.d; make -f cpp.mk clean TESTDIR=$(TESTDIR) \
		YACCRM=$(YACCRM) ; \
		cd ../pcc.d; make -f pcc.mk clean TESTDIR=$(TESTDIR) \
		YACCRM=$(YACCRM) ; \
	fi
clobber:
	-if vax  ; then cd vax; make -f cc.mk clobber TESTDIR=$(TESTDIR) \
	YACCRM=$(YACCRM) ; \
	else \
		cd cpp.d; make -f cpp.mk clobber TESTDIR=$(TESTDIR) \
		YACCRM=$(YACCRM) ; \
		cd ../pcc.d; make -f pcc.mk clobber TESTDIR=$(TESTDIR) \
		YACCRM=$(YACCRM) ; \
		rm -f cc ; \
	fi
