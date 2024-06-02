/*
 * input output utilities utilities.
 */


/*
 * Scaled down version of C Library printf.
 * Only %s %u %d (==%u) %o %x %D are recognized.
 * Used to print diagnostic information
 * directly on console tty.
 * Since it is not interrupt driven,
 * all system activities are pretty much
 * suspended.
 * Printf should not be used for chit-chat.
 */
printf(fmt, x1)
register char *fmt;
unsigned x1;
{
	register c;
	register unsigned int *adx;
	char *s;

	adx = &x1;
loop:
	while((c = *fmt++) != '%') {
		if(c == '\0') {
			return;
		}
		putchar(c);
	}
	c = *fmt++;
	if(c == 'd' || c == 'u' || c == 'o' || c == 'x')
		printn((long)*adx, c=='o'? 8: (c=='x'? 16:10));
	else if(c == 's') {
		s = (char *)*adx;
		while(c = *s++)
			putchar(c);
	} else if (c == 'D') {
		printn(*(long *)adx, 10);
		adx += (sizeof(long) / sizeof(int)) - 1;
	}
	adx++;
	goto loop;
}


/*
 * Print an unsigned integer in base b.
 */
printn(n, b)
long n;
{
	register long a;

	if (n<0) {	/* shouldn't happen */
		putchar('-');
		n = -n;
	}
	if(a = n/b)
		printn(a, b);
	putchar("0123456789ABCDEF"[(int)(n%b)]);
}


/*
 * This routine is called when there is such a bad error that 
 * there is nothing that can be done.
 */

panic(str)
char	*str;
{

	printf("SIOC PANIC: %s\n", str);
	while (1) {
	}
}
