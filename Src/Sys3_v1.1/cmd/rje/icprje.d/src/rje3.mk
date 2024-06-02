install:
	-mkdir /usr/rje3
	-chmod 755 /usr/rje3
	-chown rje /usr/rje3

	-mkdir /usr/rje3/spool
	-chmod 777 /usr/rje3/spool
	-chown rje /usr/rje3/spool

	-mkdir /usr/rje3/rpool
	-chmod 755 /usr/rje3/rpool
	-chown rje /usr/rje3/rpool

	-mkdir /usr/rje3/squeue
	-chmod 755 /usr/rje3/squeue
	-chown rje /usr/rje3/squeue

	-mkdir /usr/rje3/job
	-chmod 777 /usr/rje3/job
	-chown rje /usr/rje3/job

	-rm -f /usr/rje3/rje3disp /usr/rje3/rje3halt /usr/rje3/rje3init
	-rm -f /usr/rje3/rje3qer /usr/rje3/rje3recv /usr/rje3/rje3xmit
	-rm -f /usr/rje3/rje3load /usr/rje3/cvt /usr/rje3/snoop*

	ln /usr/rje/rjedisp /usr/rje3/rje3disp
	ln /usr/rje/rjehalt /usr/rje3/rje3halt
	ln /usr/rje/rjeinit /usr/rje3/rje3init
	ln /usr/rje/rjeqer /usr/rje3/rje3qer
	ln /usr/rje/rjerecv /usr/rje3/rje3recv
	ln /usr/rje/rjexmit /usr/rje3/rje3xmit
	ln /usr/rje/rjeload /usr/rje3/rje3load
	ln /usr/rje/cvt /usr/rje3/cvt
	ln /usr/rje/snoop /usr/rje3/snoop2
