#define	NUMMSGS	123

#ifdef	LINT

#    define	WERROR	lwerror
#    define	UERROR	luerror
#    define	MESSAGE(x)	(x)

#else

#    define	WERROR	werror
#    define	UERROR	uerror
#    define	MESSAGE(x)	msgtext[ x ]

#endif

extern char	*msgtext[ ];
