/*  */
/* @(#)unctrl.h	5.1 4/22/86 */

/*
 * unctrl.h
 *
 * @(#)unctrl.h	1.2	(1.3	8/23/82)
 * 1/26/81 (Berkeley) @(#)unctrl.h	1.1
 */

#ifndef unctrl
extern char	*_unctrl[];

# define	unctrl(ch)	(_unctrl[(unsigned) ch])
#endif
