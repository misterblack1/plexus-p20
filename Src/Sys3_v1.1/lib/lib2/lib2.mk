INCRT = $(ROOT)/usr/include
INS = /etc/install
CFLAGS = -O

SYS = Z
OBJS  = SYS.o allall.o mount.o tty.o chdir.o $(SYS)conf.o $(SYS)init.o \
	sbrk.o time.o stat.o lseek.o gtty.o stty.o isatty.o \
	getuid.o getgid.o nice.o read.o creat.o close.o \
	open.o write.o stime.o fstat.o tell.o umount.o \
	umask.o chmod.o getargv.o access.o mknod.o ustat.o \
	exit.o srcheof.o cleanup.o atol.o \
	ecvt.o filbuf.o qsort.o getopt.o strtok.o malloc.o

DEVS  = $(SYS)dk.o $(SYS)mt.o $(SYS)is.o $(SYS)rm.o $(SYS)pt.o $(SYS)pd.o \
	$(SYS)us.o $(SYS)kmem.o $(SYS)mem.o
LIBS  = lib2.a
LMODS = srt0.o lib2.a

all:	stest
stest:
	-if test "$(SYS)" = "Z"; then make -f lib2.mk comp SYS=$(SYS); \
	elif vax ; then make -f lib2.mk comp SYS=V ; \
	elif pdp11 ; then make -f lib2.mk comp SYS=70 ; fi
#	make -f lib2A.mk all; \
#	make -f lib2B.mk all ; fi

comp:	$(LMODS)

srt0.o:	$(SYS)srt0.s $(INCRT)/sys/plexus.h
	/lib/cpp -P $(SYS)srt0.s > tempfile
	as -u tempfile -o $@
	cp $@ /lib/crt2.o
	rm -f tempfile srt0.o

lib2.a:	$(OBJS) $(DEVS)
	cp /lib/libc.a lib2.a
	ar d lib2.a acct.o alarm.o cerror.o chown.o chroot.o dup.o
	ar d lib2.a execl.o execle.o execv.o execve.o execvp.o fcntl.o fork.o
	ar d lib2.a getpass.o getpid.o getppid.o kill.o link.o mktemp.o
	ar d lib2.a pause.o pipe.o popen.o profil.o ptrace.o
	ar d lib2.a setgid.o setpgrp.o setuid.o signal.o sleep.o sync.o
	ar d lib2.a syscall.o system.o tmpfile.o times.o ulimit.o uname.o
	ar d lib2.a unlink.o utime.o wait.o
	ar d lib2.a atol.o
	mkdir tmp
	mv *.o tmp
	ar x lib2.a
	cp tmp/*.o .
	rm -rf tmp
	rm -f lib2.a
	-ar rv lib2.a `lorder *.o|sed 's/^ *//'|\
	sort|comm -23 - order.out|cat order.in -|tsort`
	strip lib2.a

$(SYS)is.o: $(SYS)is.c $(INCRT)/sys/is.h
	cc -c $(CFLAGS) $(SYS)is.c

$(OBJS):	/usr/include/saio.h

$(DEVS):	/usr/include/saio.h

.c.o:
	cc -c $(CFLAGS) $<

.s.o:
	/lib/cpp -P $< > tempfile
	as -u tempfile -o $@
	rm tempfile

.o:	/usr/include/saio.h

install:	all
	$(INS) -n /usr/lib lib2.a
	rm -f /usr/lib/lib2A.a
	ln /usr/lib/lib2.a /usr/lib/lib2A.a
#	-if pdp11; then make -f lib2A.mk install;\
#	make -f lib2B.mk install; fi
clean:
	rm -f *.o
clobber: clean
	rm -f lib2.a is.h
	-if pdp11 ; then make -f lib2A.mk clobber;\
	make -f lib2B.mk clobber; fi
