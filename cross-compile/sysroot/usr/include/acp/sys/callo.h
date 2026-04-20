/* SID */
/* @(#)callo.h	5.1 4/22/86 */

/*
 * The callout structure is for a routine arranging
 * to be called by the clock interrupt
 * (clock.c) with a specified argument,
 * in a specified amount of time.
 * Used, for example, to time tab delays on typewriters.
 */

struct	callo
{
	int	c_time;		/* incremental time */
	int	c_id;		/* timer entry id */
	caddr_t	c_arg;		/* argument to routine */
	int	(*c_func)();	/* routine */
};
#ifdef ICP
struct callo callout[NCALL];
#else
extern struct callo callout[];
#endif
