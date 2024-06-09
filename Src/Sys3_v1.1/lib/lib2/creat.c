static char	sccsid[] = "@(#)creat.c	4.1";

creat(str)
char *str; {

	return (open(str, 1));
}
