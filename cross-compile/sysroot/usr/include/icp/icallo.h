/* 
 * SID @(#)icallo.h	5.1
 */

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
#ifdef VPMSYS
#ifdef ICP
	int	c_id;		/* timer entry id */
#endif
#endif
#ifdef NCF
#ifndef ICP
	int	c_id;		/* timer entry id */
#endif
#endif
	caddr_t	c_arg;		/* argument to routine */
	int	(*c_func)();	/* routine */
};
#ifdef ICP
struct callo callout[NCALL];
#else
extern struct callo callout[];
#endif
