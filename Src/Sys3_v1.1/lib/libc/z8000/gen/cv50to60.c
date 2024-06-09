/*	For converting clock tics, 50HZ, to 60HZ.  Times() syscall
 *	returns the former.  cv50to60() is declared in <sys/times.h>
 */
long cv50to60(x)
long x;
{
	return( (x*60) / 50 );
}

