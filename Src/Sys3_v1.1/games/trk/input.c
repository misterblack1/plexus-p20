int	mkfault;
int	stdin;
char	readbuf[255];
char	*readptr;
char	eof;
char	lastchar;

readchar()
{
	register char c;

	if(readptr == 0)
		newline();
	if(eof || mkfault)
		c=0;
	else if ( c = *readptr )
		 readptr++;
	return(lastchar=c);
}

newline()
{
	readptr = readbuf;
	while(eof==0&&mkfault==0&&(eof=read(stdin,readptr,1)==0,*readptr++!='\n'));
	*readptr++=0;
	readptr=readbuf;
}

flushin()
{
	readptr=0;
}

peekchar()
{
	register char c;

	c=nextchar();
	backspace();
	return(c);
}

nextchar()
{
	register char c;

	while( (c=readchar())==' ' || c=='\t' );
	return(c);
}

readint(i)
int *i;
{
	register int r;
	register char c;
	register int minus;
	int	rc;

	if((c=nextchar())=='-') {
		minus = (-1);
		c=readchar();
	} else
		minus=0;
	if(digit(c)) {
		r = 0;
		do {
			r *= 10;
			r += c - '0';
		} while(digit(c = readchar()));
		if(minus)
			r = (-r);
		*i = r;
		backspace();
		rc = 1;
	} else
		rc = 0;
	if(eof || mkfault)
		return(-1);
	else
		return(rc);
}

readreal(a)
float *a;
{
	double	r;
	int	rc;
	register	int	fr;
	register	char	c;
	register	int	minus;
	if((c=nextchar())=='-') {
		minus= (-1);
		c=readchar();
	} else
		minus=0;
	r = 0;
	rc = 0;
	if(digit(c)) {
		rc = 1;
		do {
			r *= 10;
			r += c - '0';
		} while(digit(c = readchar()));
	}
	fr = 0;
	if(c=='.') {
		while(digit(c=readchar())) {
			r *= 10;
			r += c-'0';
			fr++;
		}
		if(fr)
			rc = 1;
	}
	while(fr--)
		r /= 10;
	if(minus)
		r = (-r);
	if(rc)
		backspace();
	*a = r;
	if(eof || mkfault)
		return(-1);
	else
		return(rc);
}

reads(term, buffer)
char *term;
char buffer[];
{
	register char	c;
	register char	*ptr;

	ptr=buffer;
	while(!any(c = readchar(), term) && c)
		*ptr++ = c;
	if(mkfault || eof) {
		return(-1);
	}
	else if (ptr > buffer) {
		*ptr++ = 0;
		backspace();
		return(1);
	}
	else {
		return(0);
	}
}

backspace()
{
	if(readptr > readbuf)
		readptr--;
}

any(c, s)
char c;
char *s;
{
	register char a, b;
	register char *p;

	p=s;
	a=c;
	while(b = *p++)
		if(b == c)
			return(1);
	return(0);
}

digit(c)
char c;
{
	return(c>='0' && c<='9');
}

lineended()
{
	return(readptr==0 || *readptr==0 || peekchar()=='\n');
}

sign(a)
{
	return(a<0?-1:(a!=0));
}

cf(as1, as2)
char *as1;
char *as2;
{
	register char *s1, *s2;

	s1 = as1;
	s2 = as2;
	while(*s1++ == *s2)
		if(*s2++ = 0)
			return(0);
	return(*--s1 - *s2);
}
