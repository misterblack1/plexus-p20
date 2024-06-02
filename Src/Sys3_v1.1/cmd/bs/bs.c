/* To compile: cc -i -O atof.c bs.c string.c -lm

	BS is a compiler/interpreter
	Source in converted to reverse Polish in comp()
	and executed in execute().

	The Polish string is stored as an array of int's
	(a weakness since all kinds of pointers and doubles are stored in it).

	`Instruction' format is given below in the #define's.
	Definitions:
		S stands for the execute stack (estack)
		op is any operator
		, separates polish syllables in the instruction string
		  Syllables may be ints(operators, short const...),
		  character/function pointers(for jumps...),
		  string constants, or double constants-- all int alligned.
		N the internal array of named variables

*/
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <setjmp.h>
#define DIGIT isdigit(*Lp)
#define LETTER isalpha(*Lp)
#define LETNUM (isalpha(*Lp)||isdigit(*Lp))
#define NUMBER (isdigit(*Lp)||*Lp=='.')
#define EOL (*Lp == '\n' || !*Lp || *Lp == '#')
#define SKIP while(*Lp==' '||*Lp=='\t')++Lp
#define EQL(a,b) !strncmp(a,b,6)
#define EQS(a,b) !strcmp(a,b)
/* Table sizes */
#define NAMES		50
#define NAMEGRPS	20
#define INSTRUCTIONS	3000
#define F_STACK		50
#define E_STACK		200
#define LABELS		100
#define STRTEMPS	50
#define J_STACK		5
/* Operators & statement keywords */
#define ASG 1 /* ASG, int subscript in N */
#define LASG 2 /* LASG, int subscript of local var in S */
#define ASGSUBS 3 /* ASGSUBS, int # dimensions, int subscr in N */
/* Binary operators operate as: (S-1) op S -> --S
   arithmetic operators convert both operands to double.
   comparison operators do string compare iff both operands are strings.
   AND and OR treat non-null strings as true.
*/
#define AND 4
#define OR 5
#define EQ 6
#define NEQ 7
#define GT 8
#define LT 9
#define GEQ 10
#define LEQ 11
#define ADD 12
#define SUBT 13
#define MULT 14
#define DIV 15
#define EXPO 16
#define MOD 17
#define CAT 18
/* Unary Operators: op S -> S */
#define NOT 19
#define NEG 20
#define LINE 21 /* LINE, int statement number */
#define DUMP 22
#define TRACE 23
#define ONINTR 24 /* ONINTR, int subscr in Label */
#define EXTR 25 /* Dup top of stack: S -> ++S */
/* Primaries:  primary -> ++S */
#define DBLCONS 26 /* DBLCONS, 64 bits of double constant */
#define INTCONS 27 /* INTCONS, int-sized constant */
#define STRING 28 /* STRING, pointer to null-term string */
#define NAME 29 /* NAME, int subscr in N */
#define SUBSCR 30 /* SUBSCR, dimensions, subscr in N */
#define LNAME 31 /* LNAME, int subscr in S relative to function */
#define FUNCCALL 32 /* FUNCCALL, int subscr in Label, int arg count */
#define FUNCDEF 33 /* FUNCDEF, int decl args, int args+local var */
#define BUILTIN 34 /* BUILTIN, builtin #, # of args */
#define LIBRTN 45 /* LIBRTN, func pointer */
#define BREAK 35 /* BREAK, pointer to pointer to next instr */
#define GOTO 36 /* GOTO, pointer to next instr */
#define JUMP 37 /* JUMP, pointer */
#define IF 38 /* IF, pointer for false */
#define EXIT 39
#define RETURN 40
#define FRETURN 41
#define INTSET 42 /* INTSET, expression..., INTERROGATE */
#define INTERROGATE 43
#define SELECT 44 /* SELECT, maximum -- as in ``(0,1,2)[i]'' */
/* the following do not appear in the execute() switch */
#define IBASE 46 /* IBASE, input base (o,d,x) */
#define OBASE 47 /* OBASE, output base (o,d,x) */
#define INCR 60 /* INCR/DECR change variable values */
#define DECR 58
#define FI 62
#define CONTINUE 63 /* generates a JUMP */
#define COMPILE 48
#define EXECUTE 49
#define FOR 50
#define RUN 51
#define WHILE 52
#define STOP 53 /* uses zero for op */
#define CLEAR 54
#define NEXT 55
#define ELSE 56
#define NUF 57
#define INCLUDE 59
#define ELIF 61
#define EDIT 64
/* Misc */
#define INPUT 80
#define OUTPUT 81
#define ALLOC	127
#define ARRAY	126
#define DOUBLE 0
/* Builtin `builtins' */
#define ARG 1
#define EVAL 2
#define FORMAT 3
#define LAST 4
#define RAND 5
#define SUBSTR 6
#define INDEX 7
#define SIZE 8
#define NARG 9
#define IO 10
#define ACCESS 11
#define FTYPE 12
#define MATCH 13
#define MSTRING 14
#define TRANSLATE 15
#define GRAPH 16
#define TABLE 17
#define ITEM 18
#define KEY 19
#define ISKEY 20

#define MKDOUBLE(x) if((x)->t!=DOUBLE)mkdouble(x)
#define DBLSIZE	sizeof (double)
#define INTSIZE	sizeof (int)
#define INTBITS (INTSIZE*8-2)

char *Resnames[] = {
	"if", "else", "elif", "fi", "goto",
	"exit", "q", "return", "freturn", "for", "next", "while",
	"dump", "trace", "onintr", "run", "clear", "break", "continue",
	"fun", "nuf", "compile", "execute", "stop", "include",
	"ibase", "obase", "edit",
	0
};
char Rescode[] = {
	IF, ELSE, ELIF, FI, GOTO,
	EXIT, EXIT, RETURN, FRETURN, FOR, NEXT, WHILE,
	DUMP, TRACE, ONINTR, RUN, CLEAR, BREAK, CONTINUE,
	FUNCDEF, NUF, COMPILE, EXECUTE, STOP, INCLUDE,
	IBASE, OBASE, EDIT
};
char *Fnames[] = {
	"arg", "narg", "rand", "eval", "format", "last",
	"substr", "index", "trans", "size",
	"open", "close", "access", "ftype", "match", "mstring", "plot",
	"table", "item", "key", "iskey",
	0
};
short Fargct[] = {
	/* >=0 is exact arg count, <0 is -(N+1) minimum args */
	1, 0, 0, 1, 2, 0,
	3, 2, 3, 1,
	3, 1, 2, 1, 2, 1, -2,
	2, 2, 0, 2
};
double atan(), cos(), fabs(), exp(), log(), sin(), sqrt(), pow(), floor(),
	ceil(), fmod();
double intpow();
char Fcode[] = {
	ARG, NARG, RAND, EVAL, FORMAT, LAST,
	SUBSTR, INDEX, TRANSLATE, SIZE,
	IO, IO, ACCESS, FTYPE, MATCH, MSTRING, GRAPH,
	TABLE, ITEM, KEY, ISKEY
};
char *Libnames[] = {
	"atan", "ceil", "cos", "exp", "abs", "floor", "log", "sin", "sqrt", 0
};
double (*Libcode[])() = {
	atan, ceil, cos, exp, fabs, floor, log, sin, sqrt
};

union value {
	double	d;
	char	*sp;
	struct {
		FILE	*file;
		char	ffunc, f_or_p;
	} io;
	char	ch[DBLSIZE];
	int	intg[DBLSIZE/INTSIZE];
	struct {
		struct names	*head,
				*lastref;
	} ar_hdr;
	int	*htabl; /* really a `struct htab' */
};
struct names {
	union value v;
	char	t,
		set;
	union {
		char	sname[6];
		struct {
			int	subscr; /* stored as a complement */
			struct	names *next;
		} ar;
	} u;
} *Np[NAMEGRPS];
struct estack {
	union value v;
	char	t,
		set;
} Estack[E_STACK];
struct htitem {
	char *ht_key;
	struct estack ht_v;
};
struct htab {
	short ht_size;
	struct htitem ht_item[1];
};
struct fstack {
	int	*fip;
	struct	estack *estk, *estkp;
	char	callargs,
		tot_var,
		dclargs,
		fname;
} Fstack[F_STACK], *Fstackp = Fstack, *Ftrace = Fstack;
struct label {
	int	*l_instr;
	char	l_name[6];
} Label[LABELS];
char	Locname[10][6];
int	Lnames;
struct jump {
	int	*fail;
	jmp_buf	save;
	struct fstack *f_stackp;
	struct estack *e_stackp;
} Jmp[J_STACK], *J = Jmp;
struct jump Intr;
jmp_buf	Reset, Afterr, Remain;

short	State;
short	Cerrs = 0;
short	Trace = 0;
short	Expr = 0;
int	Instr[INSTRUCTIONS];
int	Sinstr[256];
int	*Ip = Instr;
int	*Oip = 0;
short	Namegrps = 0, LastN = 0;
short	LastS = 0;
short	Nest = 0;
int	Token;
char	*Lp;
short	Last_op = 0;
struct estack Last;
int	Lnum, Savelnum;
char	*Filename;
short	Cont;
FILE	*Input, *Output, *Sinput;
FILE	*Graphout;
int	Gio = 0;	/* Any output on plotter */
int	Ibase = 10, Obase = 10;
char	Line[512];
char	*Key;
short	Iskey;
char	*Temps[STRTEMPS];
int	Ti = -1;
int	Time[2];
double	mkdouble();
char	*to_str(), *calloc(), *malloc(), *salloc(), *substr(), *trans();
struct estack *execute();
double	atof();
extern char	*Atof;
FILE	*popen();
struct names 	*getar(), *lookup(), *nextname();
int	Argc;
char	**Argv;
struct 	stat Statb;
extern	char *Mstring[];
extern int nbra;

union gen {
	int i;
	int *ip;
	char *cp;
	struct names *np;
	struct estack *esp;
	struct fstack *fp;
	struct htab *htbl;
	struct htitem *ht;
};

intr()
{
	putchar('\n');
	signal(SIGINT, intr);
	if(Intr.e_stackp)
		longjmp(Intr.save);
	clearstk(Estack+E_STACK-1, Estack);
	fstat(fileno(stdout), &Statb);
	if((Statb.st_mode & S_IFMT) == S_IFIFO)
		exit(2);
	fstat(fileno(stdin), &Statb);
	if((Statb.st_mode & S_IFMT) == S_IFIFO)
		exit(2);
	fprintf(stderr, "ready\n");
	fclose(Input), fclose(stdin);
	Input = fopen("/dev/tty", "r");
	Sinput = 0;
	setup();
}
setup()
{
	LastS = Lnum;
	State = EXECUTE;
	Ftrace = Fstackp;
	Fstackp = Fstack;
	longjmp(Reset);
}
fpe()
{
	error("Floating exception");
}

sigpipe()
{
	error(Gio ? "Bad graph output" : "Pipe closed");
}

main(argc, argv)
char **argv;
{
	register lc;

	setjmp(Remain);
	Ip = Instr;
	Argc = argc;
	Argv = argv;
	time(Time);
	srand(Time[0]);
	Output = stdout;
	if(argc > 1) {
		Input = fopen(argv[1], "r");
		if(Input == NULL) {
			fprintf(stderr, "Cannot open %s\n", argv[1]);
			exit(1);
		}
		State = COMPILE;
	} else
		Input = stdin, State = EXECUTE;

again:
	setjmp(Reset);
	io(3, "get", 0.0, "\0", "r");
	io(3, "put", 1.0, "\0", "w");
	io(3, "puterr", 2.0, "\0", "w");
	if(signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, intr);
	signal(SIGFPE, fpe);
	signal(SIGPIPE, sigpipe);
	Lnames = 0;
	if(Sinput)
		Lnum = 0;
	else
		Lnum = Savelnum, Savelnum = 0;
	lc = 0;
	setjmp(Afterr);
	while(fgets(&Line[lc], 512-lc, Input) != NULL) {
		lc = strlen(Line) - 2;
		if(Line[lc] == '\\') {
			++Cont;
			continue;
		}
		lc = 0;
		Lp = Line;
		if(State == COMPILE)
			statement();
		else
			singstat();
	}
	Filename = 0;
	if(Input != stdin) {
		fclose(Input);
		if(Sinput) {
			Input = Sinput;
			Sinput = 0;
		}
		else {
			Input = stdin;
			State = EXECUTE;
		}
		goto again;
	}
	exit(0);
}

statement()
{
	register char *lp;
	char tname[16];

	++Lnum;
	SKIP;
	if(EOL || *Lp == '#')
		return;
	lp = Lp;
	if(aname(tname)) {
		SKIP;
		if(*Lp++ == ':')
			cklabel(tname, Ip);
		else
			Lp = lp;
	}
	if(Ip > (Instr+INSTRUCTIONS-100))
		cerror("Out of code space");
	op(LINE);
	push(Lnum);
	comp();
	if(!EOL)
		cerror("Invalid expression");
	Lnum += Cont, Cont = 0;
}
singstat()
{
	register * saveip = Ip;
	register v;
	register struct estack *ep;

	if(!Oip)
		Oip = Ip;
	if(*Lp == '!') {
		++Lp;
		fexec(Lp);
		puts("!");
		return;
	}
	if(*Lp == '#')
		return;
	Ip = Sinstr;
	Nest = 0;
	*Ip = 0;
	v = comp();
	if(!EOL)
		cerror("Invalid expression");
	push(0);
	ep = execute(Sinstr, Estack);
	if(v && Last_op!=ASG && Last_op!=ASGSUBS && *to_str(ep))
		fputs(to_str(ep), Output), putc('\n', Output);
	Lnum = 0;
	if(State == EXECUTE)
		grio();
	if(State == COMPILE && saveip >= Sinstr)
		Ip = Instr;
	else
		Ip = saveip;
}

fexec(s)
char * s;
{
	register v;
	static sts;

	if((v = fork()) == 0) { /* child */
		signal(SIGINT, SIG_DFL);
		execl("/bin/sh", "sh", "-c", s, 0);
		exit(0);
	} else if(v == -1) {
		cerror("Cannot fork");
	} else {
		signal(SIGINT, SIG_IGN);
		wait(&sts);
		signal(SIGINT, intr);
	}
}

comp()
{
	int ns;
	char *savelp;
	static *savefun;
	static struct { int tp, *loc; } s[20];
	register i;
	register union gen save1, save2, save3;
	char tname[20];

	savelp = Lp;
	save2.ip = 0;
	if(!aname(tname)) {
		return expr(0);
	}
	if((ns = ckname(tname, Resnames)) == -1) {
		Lp = savelp;
		return expr(0);
	}
	switch(Rescode[ns]) {
	case IF:
elseif:
		s[Nest].tp = IF;
elif:
		expr(1);
		op(IF);
		s[Nest++].loc = Ip++;
		SKIP;
		if(!EOL) {
			comp();
			*(s[--Nest].loc) = (int)Ip;
		}
		break;
	case ELSE:
	case ELIF:
		if(s[--Nest].tp != IF && s[Nest].tp != ELIF)
			cerror("Else: no if");
		save1.ip = s[Nest].loc;
		op(JUMP);
		s[Nest].tp = Rescode[ns];
		s[Nest++].loc = Ip++;
		*save1.ip = (int)Ip;
		if(Rescode[ns] == ELIF) {
			s[Nest].tp = ELIF;
			goto elif;
		}
		SKIP;
		if(*Lp == 'i' && *(Lp+1) == 'f') {
			Lp += 2;
			goto elseif;
		}
		break;
	case FI:
		Ip -= 2; /* get rid of LINE */
fifi:
		if(s[--Nest].tp != IF && s[Nest].tp != ELSE && s[Nest].tp != ELIF)
			cerror("Fi: no if");
		*(s[Nest].loc) = (int)Ip;
		if((s[Nest].tp == ELIF || s[Nest-1].tp == ELIF)
		&& (s[Nest-1].tp == IF || s[Nest-1].tp == ELIF))
			goto fifi;
		SKIP;
		if(*Lp == 'f' && *(Lp+1) == 'i') {
			Lp += 2;
			goto fifi;
		}
		break;
	case GOTO:
		if(aname(tname)) {
			op(GOTO);
			push(cklabel(tname, (int *)0));
			break;
		} else
			cerror("?");
	case EDIT:
		sprintf(tname, "ed %s", Argv[1]);
		fexec(tname);
		longjmp(Remain);
	case IBASE:
	case OBASE:
		SKIP;
		i = atof(Lp);
		Lp = Atof;
		if(i != 10 && i!= 8 && i != 16)
			cerror("ibase/obase must be 8, 10, or 16");
		if(Rescode[ns] == IBASE)
			Ibase = i;
		else
			Obase = i;
		break;
	case EXIT:
	case RETURN:
	case TRACE:
		if(!expr(0))
			op(INTCONS), push(0);
		op(Rescode[ns]);
		break;
	case FRETURN:
		op(FRETURN);
		break;
	case WHILE:
		s[Nest].tp = FOR;
		s[Nest++].loc = Ip;
		expr(0);
		goto forop;
	case FOR:
		/* 1st expr */
		save1.cp = Lp;
		expr(0);
		if(*Lp == ',') { /* for init,test,incr */
			++Lp;
			op(JUMP);
			save2.ip = Ip++; /* for JUMP around incr */
			s[Nest].tp = FOR;
			s[Nest++].loc = Ip;
			save3.cp = Lp; /* to rescan test */
			save1.ip = Ip;
			expr(0); /* throw away to get incr */
			Ip = save1.ip;
			if(*Lp != ',')
				goto forerr;
			++Lp;
			expr(0); /* incr */
			*save2.ip = (int)Ip;
			save2.cp = Lp;
			Lp = save3.cp; /* do test for real */
			expr(0);
			Lp = save2.cp;
		} else  { /* for init limit */
			if(Last_op!=ASG
			&& Last_op != LASG && Last_op!=ASGSUBS)
forerr:				cerror("For");
			op(JUMP); /* setup jump around incr */
			save2.ip = Ip++;
			save3.cp = Lp;
			s[Nest].tp = FOR; /* loc for jump from `next' */
			s[Nest++].loc = Ip;
			Lp = save1.cp; /* re-parse left side of 1st expr for incr */
			e9(1);
			Lp = save3.cp;
			op(INCR);
			*save2.ip = (int)Ip; /* fill in jump around init */
			save3.cp = Lp; /* re-do init */
			Lp = save1.cp;
			e9(1);
			Lp = save3.cp;
			expr(1);
			op(LEQ);
		}
forop:		op(IF);
		s[Nest].loc = Ip++; /* setup for jump out of loop */
		s[Nest++].tp = NEXT;
		SKIP; if(!EOL) {
			comp();
			goto next;
		}
		break;
	case NEXT:
		Ip -= 2; /* get rid of LINE */
next:
		if(s[--Nest].tp != NEXT)
			cerror("Next: no for");
		save1.ip = s[Nest].loc;
		op(JUMP); /* jump back to incr & test */
		*Ip++ = (int)s[--Nest].loc;
		*save1.ip = (int)Ip; /* fill in jump out of loop */
		break;
	case BREAK:
		for(i = Nest-1; i>=0; --i)
			if(s[i].tp == NEXT) {
				op(BREAK);
				*Ip++ = (int)s[i].loc;
				break;
			}
		if(i < 0)
brkerr:			cerror("No for");
		break;
	case CONTINUE:
		for(i = Nest-1; i>=0; --i)
			if(s[i].tp == FOR) {
				op(JUMP);
				*Ip++ = (int)s[i].loc;
				break;
			}
		if(i < 0)
			goto brkerr;
		break;
	case FUNCDEF:
		if(!aname(tname))
funerr:			cerror("Func def.");
		cklabel(tname, Ip);
		op(FUNCDEF); savefun = Ip++; save1.ip = Ip; Ip += 2;
		SKIP;
		i = Lnames = 0;
		if(!*Lp++ == '(') goto funerr;
		while(aname(tname)) {
			strncpy(Locname[Lnames++], tname, 6);
			++i;
			SKIP;
			if(*Lp == ',') {
				++Lp;
				continue;
			} else if(*Lp == ')') break;
			else goto funerr;
		}
		++Lp;
		while(!EOL) {
			if(aname(tname)) {
				strncpy(Locname[Lnames++], tname, 6);
				SKIP;
				if(*Lp ==',') ++Lp;
			} else goto funerr;
		}
		if(Lnames >= 10) goto funerr;
		*save1.ip = i;
		*(save1.ip+1) = Lnames;
		break;
	case NUF:
		op(INTCONS); push(0);
		op(RETURN);
		*savefun = (int)Ip;
		Lnames = 0;
		break;
	case DUMP:
		op(DUMP);
		break;
	case ONINTR:
		i = aname(tname)? cklabel(tname, (int *)0): ALLOC;
		op(ONINTR);
		push(i);
		break;
	case RUN:
		push(0);
		if(Cerrs)
			Cerrs = 0, intr();
		srand(Time[1]);
		execute(Instr, Estack);
			if(Graphout) {
				pclose(Graphout);
				Graphout = 0;
			}
		setup();
	case STOP:
		op(0);
		break;
	case CLEAR:
		clear();
		setup();
	case INCLUDE:
		Savelnum = Lnum;
		if(Sinput)
			cerror("Nested `include's");
		if(Input != stdin)
			Sinput = Input;
		Ip = Sinstr;
	case COMPILE:
		State = COMPILE;
		Lnum = 1;
		if(expr(0)) {
			save1.cp = to_str(execute(Sinstr, Estack));
			Filename = malloc(strlen(save1.cp+1));
			strcpy(Filename, save1.cp);
			Input = fopen(Filename, "r");
			if(Input == NULL) {
				Input = stdin;
				free(Filename); Filename = 0;
				cerror("Cannot open file");
			}
			*Sinstr = '\0';
			if(Rescode[ns] == INCLUDE)
				if(Ip >= Sinstr)
					Ip = Instr;
			longjmp(Reset);
		}
		if(Rescode[ns] == INCLUDE)
			cerror("Missing filename");
		if(Ip < Instr || Ip > (Instr+INSTRUCTIONS-100))
			Ip = Instr;
		break;
	case EXECUTE:
		State = EXECUTE;
		break;
	}
	return 0;
}
expr(a)
{
	register *saveip = Ip;
	register namei, subs;
	register saveop;

	e1(a);
	if(*Lp == '=') {
		++Lp;
		namei = *(Ip - 1);
		if(Last_op == NAME || Last_op == LNAME) {
			Ip -= 2;
			saveop = Last_op;
			expr(1);
			op(saveop==NAME? ASG: LASG);
			push(namei);
		 } else if(Last_op == SUBSCR) {
			subs = *(Ip - 2);
			Ip -= 3;
			expr(1);
			op(ASGSUBS);
			push(subs), push(namei);
		 } else
			Lp -= 2, cerror("Name Required");
	}
	return saveip != Ip;
}
e1(a)
{
	register i;

	e2(a);
	for(i = 0; Token && *Lp == '_'; ++i) {
		++Lp;
		e2(1);
	}
	if(i)
		op(CAT),push(i);
}
e2(a)
{
	register tp;

	e3(a);
	for(tp=0; Token;tp=0) {
		if(*Lp == '&')
			tp = AND;
		else if(*Lp == '|')
			tp = OR;
		if(tp) {
			++Lp;
			if(*Lp == '&' || *Lp == '|')
				++Lp;
			e3(1);
			op(tp);
		} else
			break;
	}
}
e3(a)
{
	register i = 0;
	register opr;

	e4(a);
	if(Token && (opr = e3a())) {
more:
		++i;
		e4(1);
		op(opr);
		if(opr = e3a()) {
			op(EXTR);
			goto more;
		}
		--i;
		while(i--)
			op(AND);
	}
}
e3a()
{
	switch(*Lp++) {
	case '>':
		if(*Lp == '=') {
			++Lp;
			return GEQ;
		} else {
			return GT;
		}
	case '<':
		if(*Lp == '=') {
			++Lp;
			return LEQ;
		} else {
			return LT;
		}
	case '=':
		if(*Lp == '=') {
			++Lp;
			return EQ;
		}
	case '!':
		if(*Lp == '=') {
			++Lp;
			return NEQ;
		}
	}
	--Lp;
	return 0;
}
e4(a)
{
	e5(a);
	for(; Token;) {
		if(*Lp == '+' && Lp[1] != '+') {
			++Lp;
			e5(1);
			op(ADD);
		} else if(*Lp == '-' && Lp[1] != '-') {
			++Lp;
			e5(1);
			op(SUBT);
		} else
			break;
	}
}
e5(a)
{
	e6(a);
	for(; Token;) {
		if(*Lp == '*') {
			++Lp;
			e6(1);
			op(MULT);
		} else if(*Lp == '/') {
			++Lp;
			e6(1);
			op(DIV);
		} else if(*Lp == '%') {
			++Lp;
			e6(1);
			op(MOD);
		} else
			break;
	}
}
e6(a)
{
	e7(a);
	while(Token && *Lp == '^') {
		++Lp;
		e7(1);
		op(EXPO);
	}
}
e7(a)
{
	register *saveip;

	SKIP;
	if(*Lp == '?') {
		++Lp;
		op(INTSET);
		saveip = Ip++;
		e8(1);
		op(INTERROGATE);
		*saveip = (int)Ip;
		return;
	}
	e8(a);
}
e8(a)
{
	register opr;

	SKIP;
	while((*Lp == '-' && *(Lp+1) != '-') || *Lp == '!') {
		opr = *Lp == '-'? NEG: NOT;
		++Lp;
		e8(1);
		op(opr);
		return;
	}
	e9(a);
}
e9(a)
{
	long cvbase();
	register i, j;
	union { double db; int intg[DBLSIZE/INTSIZE]; } dbl;
	char *cp, *cp2;
	register builtin = -1;
	register incrflg = 0;
	char tname[16];

	Token = 0;
	SKIP;
	if(*Lp == '#') /* comment */
		*Lp = '\0';
	if(EOL)
		if(a)
			goto e9err;
		else
			goto ret_false;
	if(*Lp == '(') {
		++Lp;
		if((i = rlist(')')) > 1) {
			SKIP;
			if(*Lp++ != '[')
				cerror("?");
			expr(1);
			SKIP;
			if(*Lp++ != ']')
				cerror("?");
			op(SELECT); push(i);
		}
ret:
		Token = 1;
ret_false:
		SKIP;
		return;
	}
	if(NUMBER) {
		dbl.db = Ibase==10? atof(Lp): cvbase(Ibase, Lp);
		if(Lp != Atof) /* atof succeeded */ {
			Lp = Atof;
			if(dbl.db > -(1<<INTBITS) && dbl.db < (1<<INTBITS)
			&& ((int)dbl.db == dbl.db)) { /* small int constant */
				op(INTCONS);
				push((int)dbl.db);
				goto ret;
			}
			op(DBLCONS);
			for(i = 0; i < (DBLSIZE/INTSIZE); ++i)
				push(dbl.intg[i]);
			goto ret;
		}
	}
	if(*Lp == '"') {
		op(STRING);
		cp = cp2 = salloc(128, ALLOC);
		while(*++Lp != '"')
			if(EOL)
				cerror("Unbal. quote");
			else {
				if(*Lp == '\\') {
					switch(*++Lp) {
					case '"': break;
					case 'n': *Lp = '\n'; break;
					case 'b': *Lp = '\b'; break;
					case 't': *Lp = '\t'; break;
					case 'r': *Lp = '\r'; break;
					default: --Lp;
					}
				}
				*cp2++ = *Lp;
			}
		*cp2 = '\0';
		cp2 = cp;
		cp = salloc(strlen(cp2), ALLOC);
		strcpy(cp, cp2);
		free(cp2);
		push((int)cp);
		++Lp;
		goto ret;
	}
	if(*Lp=='+' && *(Lp+1)=='+')
		Lp += 2, incrflg = 1;
	else if(*Lp=='-' && *(Lp+1)=='-')
		Lp += 2, incrflg = -1;
	if(!aname(tname)) {
		if(a || incrflg)
e9err:
			cerror("Expression");
		else
			goto ret_false;
	}
	if(ckname(tname, Resnames) != -1)
		cerror("Reserved Name in Expression");
	SKIP;
	if(*Lp == '(') {
		++Lp;
		i = rlist(')');
		if((builtin = ckname(tname, Fnames)) != -1) {
			j = Fargct[builtin];
			if((j>=0 && j!=i)
			|| (i < -(j+1)))
argerr:				cerror("Arg. count");
			op(BUILTIN);
			push(Fcode[builtin]);
			push(i);
		} else if((builtin = ckname(tname, Libnames)) != -1) {
			if(i != 1)
				goto argerr;
			op(LIBRTN);
			*Ip++ = (int)Libcode[builtin];
		} else {
			op(FUNCCALL);
			push(i);
			push(cklabel(tname, (int *)0));
		}
		goto ret;
	}
	i = 0;
	while(*Lp == '[') {
		++Lp;
		i += rlist(']');
		SKIP;
	}
	if(i) {
		op(SUBSCR);
		push(i);
		if(local(tname) != -1)
			cerror("Subscript on local name");
		push((int)lookup(tname));
	} else {
		if((i = local(tname)) != -1) {
			op(LNAME);
			push(i);
		} else {
			op(NAME);
			push((int)lookup(tname));
		}
	}
	if(incrflg)
		op(incrflg == 1? INCR: DECR);
	goto ret;
}
op(a)
register a;
{
	Last_op = a;
	*Ip++ = a;
	*Ip = 0;
}
push(v)
register v;
{
	*Ip++ = v;
	*Ip = 0;
}
cerror(s)
char *s;
{
	register i ;
	char eline[128];

	if(Expr) {
		Expr = 0;
		longjmp(J->save);
	}
	if(State == COMPILE)
		Ip = Instr;
	if((Lp-Line) > 80) {
		Line[0] = eline[0] = '\0';
		goto pskip;
	}
	for(i = 0; &Line[i] < Lp; ++i)
		eline[i] = Line[i]=='\t'? '\t': ' ';
	eline[i] = '\0';
pskip:
	if(Lnum) {
		if(Filename) {
			fprintf(stderr, "%s: ", Filename);
			Filename = 0;
		}
		fprintf(stderr, "Source line %d: ", Lnum);
	}
	fprintf(stderr, "%s\n%s%s^\b|\n", s, Line, eline);
	if(Lnum)
		Cerrs++, longjmp(Afterr);
	else
		intr();
}
aname(np)
register char *np;
{
	register i;
	SKIP;
	if(!LETTER)
		return 0;
	for(i = 1; LETNUM; ++i)
		if(i < 16)
			*np++ = *Lp++;
		else
			++Lp;
	*np = '\0';
	return 1;
}
ckname(np, table)
register char *np, **table;
{
	register i;

	for(i = 0; *table; ++i,++table)
		if(EQL(*table, np))
			return i;
	return -1;
}

long cvbase(b, s)
register char *s;
{
	register n, c;
	register long ans = 0;

	while(c = *s++) {
		if(c >= '0' && c <= '9')
			n = c - '0';
		else if(c >= 'a' && c <= 'f')
			n = c - 'a' + 10;
		else
			break;
		ans *= b;
		ans += n;
	}
	Atof = --s;
	return ans;
}
cklabel(tname, s)
register char *tname;
register *s;
{
	register i;

	for(i = 0; i < LABELS; ++i) {
		if(EQL(tname, Label[i].l_name)) {
			if(s && Label[i].l_instr)
				cerror("Dup. label");
			if(s)
				Label[i].l_instr = s;
			return i;
		} else {
			if(Label[i].l_name[0])
				continue;
			strncpy(Label[i].l_name, tname, 6);
			Label[i].l_instr = s;
			return i;
		}
	}
	cerror("Too many labels");
}
struct names *lookup(namep)
register char *namep;
{
	register i;
	register struct names *np;

	for(i = 0; i < LastN; ++i) {
		np = nextname(i);
		if(EQL(namep, np->u.sname))
			return np;
	}
	np = nextname(LastN++);
	strncpy(np->u.sname, namep, 6);
	np->v.d = 0;
	np->t = DOUBLE;
	np->set = 0;
	return np;
}
struct names *nextname(i)
register i;
{
	register nn;

	Namegrps = i / NAMES;
	nn = i % NAMES;
	if(Namegrps >= NAMEGRPS)
symerr:
		error("Symbol table overflow");
	if(!Np[Namegrps]) {
		Np[Namegrps] = (struct names *)malloc((unsigned)sizeof(struct names)*NAMES);
		if(!(int)Np[Namegrps])
			goto symerr;
	}
	return &(Np[Namegrps][nn]);
}
rlist(delim)
char delim;
{
	register int args = 0;
	for(;;) {
		SKIP;
		if(*Lp == delim) {
			++Lp;
			return args;
		}
		expr(0);
		++args;
		SKIP;
		if(*Lp == ',') {
			++Lp;
			continue;
		}
		if(*Lp != delim)
			cerror("Wrong delimiter");
	}
}
short ht_sizes[] = { 61, 113, 229, 317, 421, 521, 761, 1049 };

struct htab *htable(sz)
{
	register i;
	register struct htab *rv;

	for(i = 0; i < 8; ++i) {
		if(sz < ht_sizes[i]) {
			rv = (struct htab *)calloc(1, sizeof (short)
				+ ht_sizes[i] * sizeof(struct htitem));
			rv->ht_size = ht_sizes[i];
			return rv;
		}
	}
	error("Table too big");
}
struct htitem *htitem(h, key)
struct htab *h;
register char *key;
{
	register struct htitem *hp;
	register s, r, q;
	int first;

	for(s = r = 0; *key; ++s)
		r += *key++;
	key -= s;

	/* quadratic quotient--see CACM Feb '70, p107 */
	q = r;
	first = r = q % h->ht_size;
	q = q / h->ht_size;
	s += q;
	do {
		hp =  &(h->ht_item[r]);
		if(hp->ht_key == (char *)0) {
			if(Iskey)
				return (struct htitem *)0;
			hp->ht_key = salloc(s, ALLOC);
			strcpy(hp->ht_key, key);
			return hp;
		}
		if(EQS(key, hp->ht_key))
			return hp;
		r = (r + s) % h->ht_size;
	} while(r != first);
	error("Table overflow");
}

struct estack *execute(instr, estackp)
register *instr;
struct estack *estackp;
{
	register struct estack *estack = estackp;
	register union gen r;
	register char *s1;
	register ct;
	register char *s2;
	int x;
	double (*func)();
	double dbl;
	char *tstr;
	int opr;
	struct estack *ep;
	static struct estack *sv_estack;
	int args;

	/* A bad spot for these tests, but ... */
	if(Lnames || Nest)
		error("Open function, if, or for");
	for(;;) {
		switch(opr = *instr++) {
		case 0: /* end-of-instr */
			Last = *estack;
			return estack;
		case DBLCONS:
			++estack;
			for(r.i = 0; r.i < (DBLSIZE/INTSIZE); )
				estack->v.intg[r.i++] = *instr++;
			estack->t = DOUBLE;
			break;
		case INTCONS:
			++estack;
			estack->v.d = *instr++;
			estack->t = DOUBLE;
			break;
		case STRING:
			++estack;
			estack->v.sp = (char *)*instr++;
			estack->t = STRING;
			break;
		case LNAME:
			++estack;
			r.esp = Fstackp->estk + *instr++;
			goto nameck;
		case NAME:
			++estack;
			r.np = (struct names *)*instr++;
nameck:
			if(r.np->t==DOUBLE || r.np->t==STRING) {
				if(*instr==INCR || *instr==DECR) {
				    MKDOUBLE(r.np);
				    r.np->v.d += *instr++==INCR? 1: -1;
				    r.np->set = ALLOC;
				}
nameset:
				estack->v = r.np->v;
				estack->t = r.np->t;
			} else if(r.np->t == INPUT) {
				if(fgets(Line,512,r.np->v.io.file)==NULL) {
					if(J != &Jmp[0]) {
						sv_estack = estack;
						longjmp(J->save);
					}
					estack->v.sp = "\0";
					estack->t = STRING;
				} else {
					ct = strlen(Line);
					Line[--ct] = '\0';
					estack->v.sp = salloc(ct, 0);
					strcpy(estack->v.sp, Line);
					estack->t = STRING;
				}
			} else if(r.np->t == TABLE) {
				goto nameset;
			} else {
namerr:
				fprintf(stderr,"<%.6s>",r.np->u.sname);
				error(" name context");
			}
			break;
		case GT:
		case LT:
		case GEQ:
		case LEQ:
		case EQ:
		case NEQ:
			if((estack--)->t==STRING && estack->t==STRING)
				r.i = strcmp(estack->v.sp, (estack+1)->v.sp);
			else {
				MKDOUBLE(estack);
				MKDOUBLE(estack+1);
				r.i = (estack->v.d==(estack+1)->v.d)? 0:
					estack->v.d>(estack+1)->v.d? 1: -1;
			}
			r.i = (
				(opr==LEQ)? r.i<=0:
				(opr==EQ)? r.i==0:
				(opr==GT)? r.i>0:
				(opr==GEQ)? r.i>=0:
				(opr==LT)? r.i<0:
				r.i!=0);
			if(*instr == IF) { /* often follows RELs */
				++instr;
				goto ckif;
			}
			estack->v.d = r.i;
			estack->t = DOUBLE;
			break;
		case AND:
			r.i = true(estack--), estack->v.d = r.i&&true(estack);
			estack->t = DOUBLE;
			break;
		case OR:
			r.i = true(estack--), estack->v.d = r.i||true(estack);
			estack->t = DOUBLE;
			break;
		case ADD:
		case MULT:
		case SUBT:
		case DIV:
		case EXPO:
		case MOD:
			MKDOUBLE(estack);
			dbl = estack->v.d;
			--estack;
			MKDOUBLE(estack);
			r.i = opr;
			estack->v.d = (
				r.i==ADD? estack->v.d + dbl:
				r.i==MULT? estack->v.d * dbl:
				r.i==SUBT? estack->v.d - dbl:
				r.i==DIV? estack->v.d / dbl:
				r.i==MOD? fmod(estack->v.d, dbl):
				/* EXPO--take care of pow() */
				((floor(dbl) == dbl
				&& floor(estack->v.d) == estack->v.d)?
					intpow(estack->v.d, dbl):
					pow(estack->v.d, dbl)));
			break;
		case CAT:
			estack -= *instr;
			for(r.i = x = 0; r.i <= *instr; ++r.i)
				x += strlen(to_str(estack+r.i));
			s1 = s2 = salloc(x, 0);
			for(ct = 0; ct <= *instr; ++ct) {
				r.cp = to_str(estack+ct);
				while(*r.cp)
					*s1++ = *r.cp++;
			}
			++instr;
			*s1 = '\0';
			estack->v.sp = s2;
			estack->t = STRING;
			break;
		case EXTR:
			++estack;
			break;
		case NEG:
			estack->v.d = -mkdouble(estack);
			break;
		case NOT:
			estack->v.d = !true(estack);
			estack->t = DOUBLE;
			break;
		case LASG:
			r.esp = Fstackp->estk + *instr++;
			if(r.np->t==STRING && r.np->set==ALLOC)
					free(r.np->v.sp);
			goto asgn2;
		case ASG:
			r.np = (struct names *)*instr++;
asgn:
			switch(r.np->t) {
			case DOUBLE:
				break;
			case STRING:
				free(r.np->v.sp);
				break;
			case OUTPUT:
				grio();
				fputs(to_str(estack), r.np->v.io.file);
				if(r.np->v.io.ffunc != 'W')
					putc('\n', r.np->v.io.file);
				goto brk;
			default:
				goto namerr;
			}
asgn2:
			r.np->t = estack->t;
			if(estack->t == STRING) {
				if(estack->v.sp==Temps[Ti]) {
					Temps[Ti] = 0;
					r.np->v.sp = estack->v.sp;
				} else {
					r.np->v.sp = salloc(
					 strlen(estack->v.sp), ALLOC);
					strcpy(r.np->v.sp, estack->v.sp);
				}
			} else
				r.np->v.d = estack->v.d;
			r.np->set = ALLOC;
brk:			break;
		case ONINTR:
			if(setjmp(Intr.save)) {
				clearstk(estack, Intr.e_stackp);
				estack = estackp = Intr.e_stackp;
				Intr.e_stackp = 0;
				Fstackp = Intr.f_stackp;
				if((instr = (int *)Intr.fail)==0)
					exit(1);
				break;
			} else {
				Intr.e_stackp = estackp;
				if(*instr == ALLOC) {
					++instr;
					Intr.fail = 0;
					break;
				}
				Intr.f_stackp = Fstackp;
				if(!(Intr.fail = (int *)Label[*instr].l_instr))
					error("No label");
			}
			break;
		case INTSET:
			++J;
			if(setjmp(J->save)) {
				clearstk(sv_estack, J->e_stackp);
				estack = estackp = J->e_stackp;
				estack->v.d = 0;
				estack->t = DOUBLE;
				Fstackp = J->f_stackp;
				instr = (int *)J->fail;
				--J;
			} else {
				J->f_stackp = Fstackp;
				J->fail = (int *)*instr++;
				J->e_stackp = estackp;
			}
			break;
		case INTERROGATE:
			--J;
			estack->v.d = 1;
			estack->t = DOUBLE;
			break;
		case ASGSUBS:
		case SUBSCR:
			args = *instr++;
			r.np = (struct names *)*instr++;
			estack -= (opr == SUBSCR? args-1: args);
			if(r.np->t == TABLE) {
				if(args != 1)
					error("Only single dim. tables");
				r.ht = htitem((struct htab *)r.np->v.htabl, to_str(estack));
				r.esp = &(r.ht->ht_v);
				goto subsc;
			}
			for(ct = 0; ct < args; ++ct) {
				if(r.np->set == ALLOC)
					error("Subscript");
				r.np->t = ARRAY;
				x = mkdouble(estack+ct);
				if(x < 0)
					error("Subscr. range");
				r.np = getar(r.np, x);
			}
subsc:
			if(opr == SUBSCR)
				goto nameck;
			*estack = *(estack+args);
			goto asgn;
		case SELECT:
			r.i = mkdouble(estack);
			if(r.i >= *instr || r.i < 0)
				error("Range");
			estack -= *instr;
			*estack = *(estack + r.i);
			++instr;
			break;
		case IF:
			r.i = true(estack);
ckif:
			if(r.i)
				++instr;
			else
				instr = (int *)*instr;
			estack = estackp;
			break;
		case JUMP:
		case FUNCDEF: /* jump around func body */
			instr = (int *)*instr;
			break;
		case GOTO:
			instr = (int *)Label[*instr].l_instr;
			if(instr == 0)
				error("No label");
			break;
		case EXIT:
			if(Graphout)
				pclose(Graphout);
			exit(to_int(estack));
		case LINE:
			estack = estackp;
			if(estack > (Estack+E_STACK-15))
				error("Stack over");
			Lnum = *instr++;
			break;
		case FUNCCALL:
			ep = ++estack;
			r.fp = ++Fstackp;
			if(r.fp >= (Fstack+F_STACK-1))
				--Fstackp, error("Recursion");
			r.fp->estk = estack - *instr; /* less args */
			r.fp->estkp = estackp;
			r.fp->callargs = *instr++;
			r.fp->fip = (instr + 1);
			r.fp->fname = *instr;
			instr = (int *)Label[*instr].l_instr;
			if(*instr++ != FUNCDEF) {
				printf("Undefined function ");
				prtrace(r.fp);
				printf("Return:  ");
				Lp = salloc(128, 0);
				fgets(Lp, 128, Input);
				Ip = (int *)(tstr = salloc(128, ALLOC));
				comp();
				execute((int *)tstr, estack-1);
				free(tstr);
				goto ret;
			}
			++instr;
			r.fp->dclargs = ct = *instr++;
			r.fp->tot_var = x = *instr++;
			if(r.fp->callargs > ct)
				error("Too many args");
			estack = Fstackp->estk + ct;
			for(r.i = ct; r.i < x; ++r.i, ++estack, ++ep) {
				if(r.i < Fstackp->callargs) {
					continue;
				}
				estack->v.d = 0;
				estack->t = DOUBLE;
			}
			estack = estackp = ep;
			if(Trace)
				prtrace(Fstackp);
			break;
		case RETURN:
ret:
			Last = *estack;
			if(estack->t == STRING) {
				Last.v.sp = salloc(strlen(estack->v.sp), 0);
				strcpy(Last.v.sp, estack->v.sp);
			}
retn:
			clearstk(estack, Fstackp->estkp);
			instr = Fstackp->fip;
			estackp = Fstackp->estkp;
			estack = Fstackp->estk;
			*estack = Last;
			if(Trace) {
				--Trace;
				printf("%d: %.16s\n", (int)(Fstackp - Fstack),
					to_str(estack));
			}
			--Fstackp;
			if(Fstackp < Fstack)
				error("Return");
			break;
		case FRETURN:
			if(J != &Jmp[0]) {
				sv_estack = estack;
				longjmp(J->save);
			}
			Last.t = DOUBLE;
			Last.v.d = 0;
			goto retn;
		case BREAK:
			estack = estackp;
			instr = (int *)*instr;
			instr = (int *)*instr;
			break;
		case LIBRTN:
			func = (double (*)())*instr++;
			estack->v.d = (*func)(mkdouble(estack));
			estack->t = DOUBLE;
			break;
		case BUILTIN:
			r.i = *instr++;
			args = *instr++;
			estack -= (args - 1); /* adj. stack */
			switch(r.i) {
			case RAND:
				dbl = rand();
				dbl /= 32768.;
				estack->v.d = dbl;
				estack->t = DOUBLE;
				break;
			case ARG:
				r.i = to_int(estack);
				if(Fstackp == Fstack) { /* main func */
					if(r.i >= Argc || r.i < 0)
argerr:						error("Arg");
					estack->v.sp = Argv[r.i];
					estack->t = STRING;
					break;
				}
				if(r.i > Fstackp->callargs || r.i < 1)
					goto argerr;
				--r.i;
				*estack = *(Fstackp->estk + r.i);
				break;
			case NARG:
				estack->v.d = Fstackp==Fstack?
					Argc: Fstackp->callargs;
				estack->t = DOUBLE;
				break;
			case IO: /* open and close */
				io(args, to_str(estack),
					((estack+1)->t==DOUBLE?
						(estack+1)->v.d: 99.9),
					to_str(estack+1), to_str(estack+2));
				break;
			case EVAL:
				Expr = *instr == INTERROGATE;
				Lp = to_str(estack);
				Ip = (int *)(tstr = salloc(512, ALLOC));
				comp();
				execute((int *)tstr, estack-1);
				free(tstr);
				Expr = 0;
				break;
			case ACCESS:
				estack->v.d = access(to_str(estack),
					to_int(estack+1))==0? 1: 0;
				estack->t = DOUBLE;
				break;
			case FTYPE:
				if(stat(to_str(estack), &Statb) == -1) {
					estack->v.d = 0;
					estack->t = DOUBLE;
					break;
				}
				r.i = Statb.st_mode & S_IFMT;
				estack->v.sp = r.i&S_IFDIR? "d":
					r.i&S_IFCHR? "c":
					r.i&S_IFIFO? "p":
					r.i&S_IFBLK? "b": "f";
				estack->t = STRING;
				break;
			case LAST:
				*estack = Last;
				break;
			case SUBSTR:
				estack->v.sp = substr(to_str(estack),
					to_int(estack+1),
					to_int(estack+2));
				estack->t = STRING;
				break;
			case MATCH:
				estack->v.d = ematch(to_str(estack), to_str(estack+1));
				estack->t = DOUBLE;
				break;
			case MSTRING:
				r.i = to_int(estack);
				if(r.i<1 || r.i>10)
					goto argerr;
				estack->v.sp = r.i<=nbra? Mstring[r.i - 1]: "\0";
				estack->t = STRING;
				break;
			case INDEX:
				estack->v.d = sindex(to_str(estack),
					to_str(estack+1));
				estack->t = DOUBLE;
				break;
			case TRANSLATE:
				estack->v.sp = trans(to_str(estack),
					to_str(estack+1), to_str(estack+2));
				estack->t = STRING;
				break;
			case SIZE:
				estack->v.d = (double)strlen(to_str(estack));
				estack->t = DOUBLE;
				break;
			case FORMAT:
				sprintf((tstr=salloc(64,0)), to_str(estack),
					*(estack+1));
				estack->v.sp = tstr;
				estack->t = STRING;
				break;
			case GRAPH:
				graph(args, estack);
				estack->v.sp = "\0";
				estack->t = STRING;
				break;
			case TABLE:
				r.np = lookup(to_str(estack));
				x = mkdouble(estack+1);
				if(r.np->t)
					error("Table: allocated name");
				r.np->v.htabl = (int *)htable(x);
				r.np->t = TABLE;
				break;
			case ITEM:
				if(estack->t != TABLE)
nottab:
					error("arg not table");
				r.htbl = (struct htab *)estack->v.htabl;
				x = mkdouble(estack+1);
				if(x >= r.htbl->ht_size) {
					if(J != &Jmp[0]) {
						sv_estack = estack;
						longjmp(J->save);
					} else {
						estack->v.sp = "\0";
						estack->t = STRING;
					}
				} else {
					*estack = r.htbl->ht_item[x].ht_v;
					Key = r.htbl->ht_item[x].ht_key;
				}
				break;
			case ISKEY:
				if(estack->t != TABLE)
					goto nottab;
				++Iskey;
				s1 = to_str(estack+1);
				r.htbl = (struct htab *)estack->v.htabl;
				x = (int)htitem(r.htbl, s1);
				Iskey = 0;
				estack->t = DOUBLE;
				estack->v.d = x? 1: 0;
				break;
			case KEY:
				estack->v.sp = Key;
				estack->t = STRING;
				break;
			}
			break;
		case TRACE:
			Trace = to_int(estack);
			break;
		case DUMP:
			if(LastS)
				printf("Statement: %d\n", LastS);
			if(Ftrace > Fstack) {
				printf("Function stack:\n");
				for(; Ftrace > Fstack; --Ftrace)
					prtrace(Ftrace);
			}
			for(ct = 0; ct < LastN; ++ct) {
				r.np = nextname(ct);
				if(r.np->u.ar.subscr < 0)
					continue;
				if(r.np->t == ARRAY) {
					sprintf(Line, "%.6s",  r.np->u.sname);
					prtsubs(Line, r.np->v.ar_hdr.head);
					continue;
				}
				if(r.np->t == TABLE) {
					printf("%s: TABLE\n", r.np->u.sname);
					continue;
				}
				if(r.np->set==ALLOC) {
					printf("%.6s=%s\n",
					 r.np->u.sname, to_str(r.np));
				}
			}
			break;
		default:
			error("Compiler");
		}
		Last_op = opr;
	}
}
true(estack)
register struct estack *estack;
{
	if(estack->t == STRING)
		if(estack->v.sp[0])
			return 1;
		else
			return 0;
	else if(estack->v.d)
			return 1;
		else
			return 0;
}
struct names *getar(np, val)
struct names *np;
register val;
{
	register struct names *np1, *np2;

	val = -val;  /* subscripts are stored as complements */
	if(np1 = np->v.ar_hdr.head) {
/* shortcut for equal or +1 subscripts (didn't help)
		if((np2 = np->v.ar_hdr.lastref)->ar.subscr == val)
			return np2;
		if((np2 = np2->u.ar.next)->subscr == val)
			return np->v.ar_hdr.lastref = np2;
*/
loop:
		if(np1->u.ar.subscr == val) {
			np->v.ar_hdr.lastref = np1;
			return np1;
		}
		if(np2 = np1->u.ar.next ) {
			np1 = np2;
			goto loop;
		}
		np2 = nextname(LastN++);
		np1->u.ar.next = np2;
set:
		np2->u.ar.next = (struct names *)0;
		np2->u.ar.subscr = val;
		np2->v.d = 0.0;
		np2->set = 0;
		np2->t = DOUBLE;
		np->v.ar_hdr.lastref = np2;
		return np2;
	}
	np2 = np->v.ar_hdr.head = nextname(LastN++);
	goto set;
}
error(s)
register char *s;
{
	if(Expr) {
		Expr = 0;
		longjmp(J->save);
	}
	fprintf(stderr, "%s - execution error", s);
	if(Lnum)
		fprintf(stderr, " in source line %d", Lnum);
	fprintf(stderr, "\n");
	intr();
}
prtsubs(ps, np)
register char *ps;
register struct names *np;
{
	register char *lc;

	lc = ps + strlen(ps);
	for(;;) {
		*lc = '\0';
		sprintf(ps, "%s[%d]", ps, -(np->u.ar.subscr));
		if(np->set == ALLOC)
			printf("%s=%s\n", ps, to_str(np));
		else if(np->t == ARRAY)
			prtsubs(ps, np->v.ar_hdr.head);
		if(!np->u.ar.next)
			return;
		np = np->u.ar.next;
	}
}
double mkdouble(estack)
register struct estack *estack;
{
	if(estack->t == DOUBLE)
		return estack->v.d;
	if(estack->t==INPUT || estack->t==OUTPUT)
		goto conv;
	else if(estack->t == STRING) {
		register char *s = estack->v.sp, *strend;

		strend = strlen(s) + s;
		estack->v.d = Ibase==10? atof(s): cvbase(Ibase, s);
		if(estack->set == ALLOC)
			free(s);
		if(Atof != strend)
conv:			error("Conversion");
		estack->t = DOUBLE;
		return estack->v.d;
	}
}
to_int(estack)
register struct estack *estack;
{
	return (int)(estack->t==DOUBLE? estack->v.d: mkdouble(estack));
}
char *to_str(estack)
register struct estack *estack;
{
	register char *s, *st;
	static char rv[3][42];
	static which = 0;

	st = rv[which = which==2? 0: ++which];
	if(estack->t == DOUBLE) {
		if(Obase == 10)
			sprintf(st, "%.10f", estack->v.d);
		else  {
			sprintf(st, (Obase==8? "%lo": "%lx"), (long)estack->v.d);
			return st;
		}
		s = st + strlen(st);
		while(*--s != '.')
			if(*s == '0') {
				*s = '\0';
				continue;
			} else
				return st;
		*s = '\0';
		return st;
	} else if(estack->t==INPUT || estack->t==OUTPUT)
		return "I/O";
	else
		return estack->v.sp;
}
char *salloc(ct, t)
register ct, t;
{
	register char *rv;

	if(rv = malloc((unsigned)(ct+1))) {
		if(t != ALLOC) { /* temp */
			if(!(++Ti < STRTEMPS))
				Ti = 0;
			if(Temps[Ti]) {
				free(Temps[Ti]);
			}
			Temps[Ti] = rv;
		}
		return rv;
	}
	error("Out of string space");
}
io(args, ns, fd, flname, func)
char *ns;
double fd;
register char *flname, *func;
{
	FILE *fl;
	int i;
	register struct names *np;

	np = lookup(ns);
	if(np->t==INPUT || np->t==OUTPUT) {
		fl = np->v.io.file;
		if(fl!=stdin && fl!=stdout && fl!=stderr)
			fclose(fl);
		if(np->v.io.f_or_p == '!')
			wait(&i);
		np->v.d = np->set = 0;
		np->t = DOUBLE;
	}
	if(args == 1)
		return;
	switch(np->v.io.ffunc = *func) {
	case 'r':
		if(*flname == '!')
			np->v.io.file = popen(&flname[1], func);
		else if (fd == 0)
			np->v.io.file = stdin, flname = 0;
		else
			np->v.io.file = fopen(flname, func);
		if(np->v.io.file == NULL)
accerr:			error("Cannot access file");
		np->t = INPUT;
		break;
	case 'W': /* no CR */
		*func = 'w';
	case 'w':
	case 'a':
		if(fd == 1)
			np->v.io.file = stdout, flname = 0;
		else if(fd == 2)
			np->v.io.file = stderr, flname = 0;
		else if(*flname == '!')
			np->v.io.file = popen(&flname[1], func);
		else
			np->v.io.file = fopen(flname, func);
		if(np->v.io.file == NULL)
			goto accerr;
		np->t = OUTPUT;
		break;
	default:
		error("Last arg to open() must be r, w, or a");
	}
	np->v.io.f_or_p = flname[0];
	np->set = ALLOC;
}
clear()
{
	register i, j;
	register struct names *np;

	Fstackp = Fstack;
	for(i = 0; i < LABELS; ++i) {
		Label[i].l_instr = 0;
		for(j = 0; j < 6; ++j)
			Label[i].l_name[j] = '\0';
	}
	for(i = 0; i < LastN; ++i) {
		np = nextname(i);
		if(np->t==INPUT || np->t==OUTPUT) {
			io(1, np->u.sname);
		} else if(np->t == STRING)
			free(np->v.sp);
		np->v.d = 0;
		np->t = DOUBLE;
		np->u.sname[0] = '\0';
	}
	Instr[0] = '\0';
	Ip = Instr;
	Trace = Namegrps = LastN = Lnum = 0;
}
local(tname)
register char *tname;
{
	register i;

	for(i = 0; i < Lnames; ++i) {
		if(EQL(tname, Locname[i]))
			return i;
	}
	return -1;
}
prtrace(fs)
register struct fstack *fs;
{
	register struct estack *estack;
	register i;

	printf("%d: ", (int)(fs - Fstack));
	printf("%.6s(", Label[fs->fname].l_name);
	estack = fs->estk;
	for(i = 0; i < fs->callargs; ++i) {
		if(i)
			fputs(", ", stdout);
		printf("%.20s", to_str(estack+i));
	}
	fputs(") ", stdout);
	for(i = fs->dclargs; i < fs->tot_var; ++i) {
		if(i > fs->dclargs)
			fputs(", ", stdout);
		printf("%.20s", to_str(estack+i));
	}
	fputs("\n", stdout);
}
double intpow(a, b)
register double a, b;
{
	register double t;

	t = a;
	while(--b)
		a *= t;
	return a;
}
clearstk(top, bottom)
register struct estack *top, *bottom;
{
	while(bottom <= top) {
		if(bottom->set == ALLOC) {
			if(bottom->t == STRING)
				free(bottom->v.sp);
			bottom->set = 0;
		}
		++bottom;
	}
}

/*
 * Graphic function table - argcount is number of
 * args in the user program.  Arg count of 2 
 * (label, linemod) have only a string argument.
 */

struct gr
{
	char	nargs;
	char	code;
} gr[] = {
	2,	0,	/* Open plotter */
	1,	'e',	/* Erase */
	2,	't',	/* Label */
	5,	'l',	/* Line */
	4,	'c',	/* Circle */
	7,	'a',	/* Arc */
	6,	0,	/* Dot - not implemented */
	3,	'm',	/* Move */
	3,	'n',	/* Cont */
	3,	'p',	/* Point */
	2,	'f',	/* Linemod */
	5,	's',	/* Space */
	5,	0,	/* Scale and shift */
};

#define NGRAPH	14

double	xscale = 1.0;
double	yscale = 1.0;
double	xshift = 0.0;
double	yshift = 0.0;

char *Plot = "tplot -TXXXXXXXXXXX";

graph(args, estack)
register struct estack *estack;
{
	register fcn;
	register char *stp;

	fcn = to_int(estack);
	if(fcn >= NGRAPH || fcn < 0)
		error("Illegal graph function");
	if(args != gr[fcn].nargs)
		error("graph: arg count");
	if(fcn && Graphout==0)
		error("Graphic output never specified");
	estack++;

	Gio++;
	if(fcn == 0) {		/* pipe output through plot(I) */
		Plot[7] = '\0';
		if(strlen(stp = to_str(estack)) >  10)
			error("graph: Bad dest.");
		strcat(Plot, stp);
		grio();
		return;
	}
	if(gr[fcn].code)
		putgr(fcn, estack);
	else
		switch(fcn) {
		case 6:		/* Dot */
			error("Dot not implemented");
			return;


		case 12:	/* Scale*/
			xscale = mkdouble(estack++);
			yscale = mkdouble(estack++);
			xshift = mkdouble(estack++);
			yshift = mkdouble(estack);
			return;

		}
}

putgr(n, estack)
register struct estack *estack;
{
	register narg, i;
	double loc;

	narg = gr[n].nargs;
	putc(gr[n].code, Graphout);
	if(narg == 2) {
		fputs(to_str(estack), Graphout);
		putc('\n', Graphout);
		return;
	}
	for(i = 0, narg -= 2; i <= narg; i++) {
		loc = mkdouble(estack++);
		if(i & 01)
			putwd((int)(loc * xscale + xshift));
		else if(i == narg)
			putwd((int)(loc * xscale));
		else
			putwd((int)(loc * yscale + yshift));
	}
}

grio()
{
	if(!Gio)
		return;
	if(Graphout)
		pclose(Graphout);
	Graphout = popen(Plot, "w");
	if(Graphout == NULL)
		error("No plot!");
	Gio = 0;
}

putwd(a)
{
	union {
		struct { char lo, hi; } chs;
		int wd;
	} w;

	w.wd = a;
	putc(w.chs.lo, Graphout);
	putc(w.chs.hi, Graphout);
}
