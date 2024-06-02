/*
 * Copy N bytes of data from SRC to DST
 */
copy(src,dst,n)
register char *src,*dst;
register int n;
{
	do {
		*dst++ = *src++;
	} while(--n);
}
