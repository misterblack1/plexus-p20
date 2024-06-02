typedef struct sty STYPE;
struct sty { ATYPE t; STYPE *next; };

typedef struct sym {
	char name[LCHNM];
	char nargs;
	int decflag;
	int fline;
	STYPE symty;
	int fno;
	int use;
	} STAB;
