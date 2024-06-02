static char	sccsid[] = "@(#)stime.c	4.1";

stime(tloc)
long *tloc; {
	extern long _time;

	_time = *tloc;
}
