
/* SID @(#)kmc.h	5.1 */
/* @(#)kmc.h	6.1 */

#define	KSTEP	1
#define	KMS	2
#define	KCSR	3
#define	KSTOP	4
#define	KMCLR	5
#define	KRUN	6
#define	KLU	7
#define	KWRCR	8
#define	KRESET	9

#define	KCSETA	(('k'<<8)|1)
 
#define OKCMD 	15
#define OKCHECK	15

#define OKOPT	01
#define OKCHK	02

struct kmcntl {
	int	kmd;
	short	*kcsr;
	int	kval;
};

struct kmc {
	struct	clist k_inq;
	short	k_stat;
	char	k_type;
	char	k_opt;
	short	k_arg[3];
	int	(*k_rint)();
};

