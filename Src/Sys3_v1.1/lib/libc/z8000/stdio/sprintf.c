#include <stdio.h>
#include <varargs.h>
#include "param.h"

int _print();
extern FILE *_pfile;

int
sprintf (va_alist) va_dcl
{
	register char *format;
	FILE strbuf;
	register int rc;
	register char *string;
	va_list ap;

	va_start (ap);
	string = va_arg (ap, char *);
	strbuf._flag = _IOWRT + _IOSTRG;
	strbuf._ptr = string;
	strbuf._cnt = MAXINT;
	format = va_arg (ap, char *);
	_pfile = &strbuf;
	rc = _print (format, &ap);
	va_end (ap);
	*strbuf._ptr = '\0';
	return rc;
}
