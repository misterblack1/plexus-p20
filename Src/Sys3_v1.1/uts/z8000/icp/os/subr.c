#ifdef COUNT
extern int profile[];
#endif
/*
 * Return the logical maximum
 * of the 2 arguments.
 */
max(a, b)
unsigned a, b;
{
#ifdef COUNT
	profile[70]++;
#endif

	if(a > b)
		return(a);
	return(b);
}


/*
 * Return the logical mimimum
 * of the 2 arguments.
 */
min(a, b)
unsigned a, b;
{

#ifdef COUNT
	profile[71]++;
#endif

	if(a < b)
		return(a);
	return(b);
}


/*
 * Routine which sets a user error; placed in
 * illegal entries in the bdevsw and cdevsw tables.
 */
nodev()
{
#ifdef COUNT
	profile[72]++;
#endif

}

/*
 * Null routine; placed in insignificant entries
 * in the bdevsw and cdevsw tables.
 */
nulldev()
{
#ifdef COUNT
	profile[73]++;
#endif

}
