CFLAGS = -O
INS=/etc/install

FILES = bls \
	dial \
	xstr \
	mkstr \
	head \
	strings \
	ctags \
	printenv \
	dumpdir \
	arcv6 \
	copytape \
	clear \
	script \
	bbanner \
	tape

all:	termlib.a $(FILES)
	cd tset;make -f tset.mk tset "LDFLAGS=$(LDFLAGS)"
	cd more;make -f more.mk more "LDFLAGS=$(LDFLAGS)"
	cd csh; make -f csh.mk csh "LDFLAGS=$(LDFLAGS)"
	cd ex;make -f vi.mk vi "LDFLAGS=$(LDFLAGS)"

termlib.a:
	cd termlib;make -f termlib.mk install "LDFLAGS=$(LDFLAGS)"

tape: tape.o
	cc $(LDFLAGS) -o tape tape.o

bls.o:\
	$(ROOT)/usr/include/sys/param.h\
	$(ROOT)/usr/include/sys/stat.h\
	$(ROOT)/usr/include/sys/dir.h\
	$(ROOT)/usr/include/stdio.h\
	$(ROOT)/usr/include/ctype.h\
	$(ROOT)/usr/include/pwd.h\
	$(ROOT)/usr/include/grp.h\
	$(ROOT)/usr/include/utmp.h\
	$(ROOT)/usr/include/sgtty.h\
	$(ROOT)/usr/include/varargs.h
	cc $(CFLAGS) -c $<

bbanner bbanner.o: $(ROOT)/usr/include/stdio.h
	cc -o bbanner bbanner.c

xstr.o:\
	$(ROOT)/usr/include/stdio.h\
	$(ROOT)/usr/include/ctype.h\
	$(ROOT)/usr/include/sys/types.h\
	$(ROOT)/usr/include/signal.h
	cc $(CFLAGS) -c $<

head.o:\
	$(ROOT)/usr/include/stdio.h
	cc $(CFLAGS) -c $<

mkstr.o:\
	$(ROOT)/usr/include/stdio.h\
	$(ROOT)/usr/include/sys/types.h\
	$(ROOT)/usr/include/sys/stat.h
	cc $(CFLAGS) -c $<

strings.o:\
	$(ROOT)/usr/include/stdio.h\
	$(ROOT)/usr/include/a.out.h\
	$(ROOT)/usr/include/ctype.h
	cc $(CFLAGS) -c $<

ctags.o:\
	$(ROOT)/usr/include/stdio.h\
	$(ROOT)/usr/include/ctype.h
	cc $(CFLAGS) -c $<

printenv.o:
	cc $(CFLAGS) -c $<

dial.o:
	cc $(CFLAGS) -c $<

copytape.o:
	cc $(CFLAGS) -c $<

dumpdir.o:
	cc $(CFLAGS) -c $<

arcv6.o:
	cc $(CFLAGS) -c $<

clear : clear.o
	cc $(LDFLAGS) -o clear clear.o -ltermlib
clear.o:
	cc $(CFLAGS) -c $<

script.o:
	cc $(CFLAGS) -c $<

clean: clobber

clobber:
	-rm -f $(FILES)
	-rm -f *.o
	cd more;make -f more.mk clobber
	cd csh; make -f csh.mk clobber
	cd tset; make -f tset.mk clobber
	cd ex; make -f vi.mk clobber

install: all 
	$(INS) -f /usr/plx bls
	$(INS) -f /usr/plx dial
	$(INS) -f /usr/plx xstr
	$(INS) -f /usr/plx clear
	$(INS) -f /usr/plx script
	$(INS) -f /usr/plx mkstr
	$(INS) -f /usr/plx head
	$(INS) -f /usr/plx bbanner
	$(INS) -f /usr/plx copytape
	$(INS) -f /usr/plx strings
	$(INS) -f /usr/plx printenv
	$(INS) -f /usr/plx dumpdir
	$(INS) -f /usr/plx arcv6
	$(INS) -f /usr/plx ctags
	$(INS) -f /usr/plx tape
	cd tset;make -f tset.mk install "LDFLAGS=$(LDFLAGS)"
	cd more;make -f more.mk install "LDFLAGS=$(LDFLAGS)"
	cd csh; make -f csh.mk install "LDFLAGS=$(LDFLAGS)"
	cd ex;make -f vi.mk install "LDFLAGS=$(LDFLAGS)";\
		make -f vi.mk installutil "LDFLAGS=$(LDFLAGS)"
