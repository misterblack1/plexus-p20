static char	sccsid[] = "@(#)exit.c	4.1";

#ifdef STOCKIII
exit ()
{
#else
exit(exitval)
{
	printf("Exit %d\n",exitval);
#endif
	_cleanup ();
	_exit ();
}
