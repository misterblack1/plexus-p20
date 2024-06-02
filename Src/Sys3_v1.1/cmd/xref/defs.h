#

#define SRBHDR

#define LOCAL	static
#define TYPE	typedef
#define STRUCT	TYPE struct
#define UNION	TYPE union
#define REG	register

#define IF	if(
#define THEN	){
#define ELSE	} else {
#define ELIF	} else if (
#define FI	}

#define BEGIN	{
#define END	}
#define SWITCH	switch(
#define IN	){
#define ENDSW	}
#define FOR	for(
#define WHILE	while(
#define DO	){
#define OD	}
#define REP	do{
#define PER	}while(
#define DONE	);
#define LOOP	for(;;){
#define POOL	}


#define SKIP	;
#define DIV	/
#define REM	%
#define NEQ	^
#define ANDF	&&
#define ORF	||

#define TRUE	(-1)
#define FALSE	0
#define LOBYTE	0377

#define NL	'\n'
#define SP	' '
#define TB	'\t'
#define MAX(a,b)	((a)>(b)?(a):(b))


TYPE	int		INT;
TYPE	long INT	L_INT;
TYPE	unsigned int	POS;
TYPE	unsigned short	S_POS;
TYPE	int		VOID;
TYPE	float		REAL;
TYPE	double		L_REAL;
TYPE	char		BOOL;
TYPE	char		CHAR;
TYPE	char		*STRING;
TYPE	char		MSG[];
TYPE	int		(*FNVAR)();
TYPE	FNVAR		SIGARG;	/*temp until unions work*/
TYPE	int		UFD;
