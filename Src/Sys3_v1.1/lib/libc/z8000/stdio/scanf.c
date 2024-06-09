/*LINTLIBRARY*/
#include	<stdio.h>

/*VARARGS1*/
scanf(fmt, args)
char *fmt;
{
	return(_doscan(stdin, fmt, &args));
}

/*VARARGS2*/
fscanf(iop, fmt, args)
FILE *iop;
char *fmt;
{
	return(_doscan(iop, fmt, &args));
}

/*VARARGS2*/
sscanf(str, fmt, args)
register char *str;
char *fmt;
{
	FILE _strbuf;

	_strbuf._flag = _IOREAD|_IOSTRG;
	_strbuf._ptr = _strbuf._base = str;
	_strbuf._cnt = 0;
	while (*str++)
		_strbuf._cnt++;
	return(_doscan(&_strbuf, fmt, &args));
}
