#
/*
 * input library
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include	"input.h"

INT		peekc;
INT		eof;
FILEBLK		_stdfile;
FILE		standin = &_stdfile;


readc()
{
	REG INT		c;
	REG INT		len;
	REG FILE	f;

retry:
	IF peekc
	THEN	c=peekc&LOBYTE; peekc=0;
	ELIF (f=standin, f->fnxt!=f->fend)
	THEN	c = *f->fnxt++;
		IF c==NL THEN f->flin++; FI
	ELIF eof ORF f->fdes<0
	THEN	c=EOF; eof++;
	ELIF (len=read(f->fdes,f->fbuf,f->fsiz))<=0
	THEN	close(f->fdes); f->fdes = -1; c=EOF; eof++;
	ELSE	f->fend = (f->fnxt = f->fbuf)+len;
		goto retry;
	FI
	return(c);
}

initf(fd)
	UFD		fd;
{
	REG FILE	f=standin;

	f->fdes=fd;
	f->fsiz=BUFSIZ;
	f->fnxt=f->fend=f->fbuf; f->flin=1;
	eof=FALSE;
}

estabf(s)
	REG STRING	s;
{
	REG FILE	f;

	(f=standin)->fdes = -1;
	f->fend=length(s)+(f->fnxt=s);
	f->flin=1;
	return(eof=(s==0));
}

push(af)
	FILE		af;
{
	REG FILE	f;

	(f=af)->fstak=standin;
	f->feof=eof;
	standin=f;
}

pop()
{
	REG FILE	f;

	IF (f=standin)->fstak
	THEN	IF f->fdes>=0 THEN close(f->fdes); FI
		standin=f->fstak; eof=f->feof;
		return(TRUE);
	ELSE	return(FALSE);
	FI
}
