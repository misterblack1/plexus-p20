	/lib/cpp -P Zsrt0.s > tempfile
	as -u tempfile -o srt0.o
	cp srt0.o /lib/crt2.o
	rm -f tempfile srt0.o
	cc -c -O SYS.c
	cc -c -O atol.c
	cc -c -O Zis.c
	cc -c -O Zpd.c
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
	ar rv lib2.a `lorder *.o|sed 's/^ *//'|\
	sort|comm -23 - order.out|cat order.in -|tsort`
tsort: cycle in data
tsort: crt2.o
tsort: Zinit.o
tsort: perror.o
tsort: write.o
tsort: tty.o
tsort: cuexit.o
tsort: cycle in data
tsort: crt2.o
tsort: Zinit.o
tsort: mknod.o
tsort: tty.o
tsort: cuexit.o
tsort: cycle in data
tsort: strncpy.o
tsort: crt2.o
tsort: Zinit.o
tsort: sprintf.o
tsort: _print.o
tsort: flsbuf.o
tsort: cuexit.o
tsort: cycle in data
tsort: Zmt.o
tsort: Zrm.o
tsort: printf.o
tsort: _print.o
tsort: flsbuf.o
tsort: cuexit.o
tsort: cycle in data
tsort: Zmt.o
tsort: Zpt.o
tsort: printf.o
tsort: _print.o
tsort: flsbuf.o
tsort: cuexit.o
tsort: cycle in data
tsort: Zdk.o
tsort: Zpd.o
tsort: printf.o
tsort: _print.o
tsort: flsbuf.o
tsort: cuexit.o
tsort: cycle in data
tsort: Zdk.o
tsort: Zis.o
tsort: printf.o
tsort: _print.o
tsort: flsbuf.o
tsort: cuexit.o
tsort: cycle in data
tsort: strncmp.o
tsort: crt2.o
tsort: SYS.o
tsort: printf.o
tsort: _print.o
tsort: flsbuf.o
tsort: cuexit.o
tsort: cycle in data
tsort: crt2.o
tsort: close.o
tsort: Zconf.o
tsort: cycle in data
tsort: Zkmem.o
tsort: Zconf.o
tsort: cycle in data
tsort: crt2.o
tsort: allall.o
tsort: cret.o
a - Zmem.o
a - Zus.o
a - a64l.o
a - abort.o
a - abs.o
a - access.o
a - bsearch.o
a - calloc.o
a - chdir.o
a - clrerr.o
a - crypt.o
a - ctermid.o
a - ctime.o
a - getenv.o
a - cuserid.o
a - getlogin.o
a - getpwuid.o
a - getuid.o
a - cv50to60.o
a - fdopen.o
a - fgetc.o
a - fltunused.o
a - fltused.o
a - freopen.o
a - fseek.o
a - ftell.o
a - gcvt.o
a - getargv.o
a - gets.o
a - srcheof.o
a - strtok.o
a - getchar.o
a - getegid.o
a - geteuid.o
a - getgid.o
a - getgrgid.o
a - getgrnam.o
a - getgrent.o
a - ttyslot.o
a - getopt.o
a - fputc.o
a - fputs.o
a - getpw.o
a - getpwnam.o
a - getpwent.o
a - fgets.o
a - fopen.o
a - endopen.o
a - chmod.o
a - umask.o
a - findiop.o
a - rew.o
a - getw.o
a - gtty.o
a - ioctl.o
a - l64a.o
a - locking.o
a - lsearch.o
a - mcount.o
a - mon.o
a - creat.o
a - nice.o
a - nlist.o
a - putchar.o
a - putpwent.o
a - fprintf.o
a - puts.o
a - putw.o
a - qsort.o
a - rand.o
a - rdwr.o
a - rmount.o
a - rumount.o
a - scanf.o
a - doscan.o
a - atof.o
a - filbuf.o
a - ungetc.o
a - setbuf.o
a - setjmp.o
a - ssignal.o
a - stime.o
a - strcspn.o
a - strncat.o
a - strpbrk.o
a - strspn.o
a - stty.o
a - swab.o
a - tell.o
a - tmpnam.o
a - tolower.o
a - toupper.o
a - ttyname.o
a - stat.o
a - fstat.o
a - time.o
a - strcat.o
a - umount.o
a - ustat.o
a - open.o
a - lseek.o
a - read.o
a - strrchr.o
a - vfprintf.o
a - vprintf.o
a - vsprintf.o
a - write.o
a - tty.o
a - exit.o
a - Zinit.o
a - atoi.o
a - atol.o
a - mknod.o
a - mount.o
a - perror.o
a - sprintf.o
a - strcmp.o
a - strncpy.o
a - errlst.o
a - Zrm.o
a - Zpt.o
a -ar: creating lib2.a
 Zpd.o
a - Zis.o
a - SYS.o
a - l3.o
a - printf.o
a - strchr.o
a - strcpy.o
a - strncmp.o
a - _fprint.o
a - ecvt.o
a - modf.o
a - ldexp.o
a - frexp.o
a - _print.o
a - flsbuf.o
a - cuexit.o
a - fakcu.o
a - malloc.o
a - switch.o
a - ctype_.o
a - strlen.o
a - data.o
a - isatty.o
a - sbrk.o
a - close.o
a - Zconf.o
a - Zdk.o
a - Zkmem.o
a - Zmt.o
a - allall.o
a - cret.o
a - crt2.o
a - csav.o
a - cleanup.o
	strip lib2.a
