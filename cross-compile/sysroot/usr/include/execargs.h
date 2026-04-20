/*  */
/* @(#)execargs.h	5.1 4/22/86 */

/*	@(#)execargs.h	1.2	*/
#if vax
char **execargs = (char**)(0x7ffffffc);
#endif

#if m68
char **execargs = (char**)(0x1ffffc);
#endif

#if pdp11
char **execargs = (char**)(-2);
#endif

#if u3b || u3b5
/* empty till we can figure out what to do for the shell */
#endif
