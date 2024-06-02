#include <stdio.h>
#include <varargs.h>
#include "param.h"

int _print();
extern FILE *_pfile;

int
vsprintf (string, format, args)
	register char *string, *format;
	va_list args;
{
	FILE strbuf;
	register int rc;

	strbuf._flag = _IOWRT + _IOSTRG;
	strbuf._ptr = string;
	strbuf._cnt = MAXINT;
	_pfile = &strbuf;
	rc = _print (format, &args);
	*strbuf._ptr = '\0';
	return rc;
}
