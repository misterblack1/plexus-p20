static char Sccsid[]="@(#)verify	3.2";
/*
	If `astr1' contains any characters not in `str2' return the
	offset of the first such character found in `astr1',
	else return -1.
*/

verify(astr1,str2)
char *astr1;
register char *str2;
{
	register char *str1;

	for (str1=astr1; *str1; str1++)
		if (!any(*str1,str2))
			return(str1 - astr1);
	return(-1);
}
