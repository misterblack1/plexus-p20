	/user/craig/bin/cc -O -DLINT -Imip -I. -DBUG4 -c lerror.c
	/user/craig/bin/cc -O -DLINT -Imip -I. -DBUG4 -c msgbuf.c
	/user/craig/bin/cc -O -DLINT -Imip -I. -DBUG4 -c lint.c
	/user/craig/bin/cc -i -o lint1 trees.o pftn.o scan.o comm1.o messages.o optim.o xdefs.o cgram.o lerror.o msgbuf.o lint.o
	/user/craig/bin/cc -O -DLINT -Imip -I. -DBUG4 -c lpass2.c
	/user/craig/bin/cc -O -DLINT -Imip -I. -DBUG4 -c lerror2.c
	/user/craig/bin/cc -O -DLINT -Imip -I. -DBUG4 -c msgbuf2.c
	/user/craig/bin/cc -i -o lint2 lpass2.o lerror2.o messages.o msgbuf2.o
	/lib/cpp -C llib-lc | ./lint1 -vx -H/usr/tmp/hlint >llib-lc.ln 
	rm /usr/tmp/hlint
	/lib/cpp -C llib-port | ./lint1 -vxp -H/usr/tmp/hlint >llib-port.ln 
	rm /usr/tmp/hlint 
