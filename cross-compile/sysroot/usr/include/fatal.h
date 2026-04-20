/*  */
/* @(#)fatal.h	5.1 4/22/86 */

/*	@(#)fatal.h	1.2	*/
extern	int	Fflags;
extern	char	*Ffile;
extern	int	Fvalue;
extern	int	(*Ffunc)();
#ifdef m68
extern	int	Fjmp[13];
#else
extern	int	Fjmp[10];
#endif

# define FTLMSG		0100000
# define FTLCLN		 040000
# define FTLFUNC	 020000
# define FTLACT		    077
# define FTLJMP		     02
# define FTLEXIT	     01
# define FTLRET		      0

# define FSAVE(val)	SAVE(Fflags,old_Fflags); Fflags = val;
# define FRSTR()	RSTR(Fflags,old_Fflags);
