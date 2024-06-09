static char Sccsid[]="@(#)strend	3.1";
char *strend(p)
register char *p;
{
	while (*p++)
		;
	return(--p);
}
