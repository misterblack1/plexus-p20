/*
 * Header for object code improver
 */

#include <stdio.h>

#define	JP	1
#define	CJP	2
#define	JR	3
#define CJR	4
#define	LABEL	5
#define	DLABEL	6
#define NLABEL	7
#define	EROU	8
#define	JSW	9
#define	LD	10
#define	CLR	11
#define	COM	12
#define	INC	13
#define	DEC	14
#define	NEG	15
#define	TST	16
#define	SDA	17
#define	SDL	18
#define	EXTS	19
#define	CP	20
#define	ADD	21
#define	SUB	22
#define	AND	24
#define	OR	25
#define	MUL	26
#define	DIV	27
#define	XOR	29
#define	BIT	30
#define SET	31
#define RES	32
#define LDA	33
#define LDM	34
#define LDK	35
#define LDIR	36
#define PUSH	37
#define POP	38
#define TCC	39
#define SLA	40
#define SLL	41
#define SRA	42
#define SRL	43
#define	TEXT	50
#define	DATA	51
#define	BSS	52
#define ASCII	53
#define	EVEN	54
#define WORD	55
#define DBYTE	56
#define	ADDF	60
#define	SUBF	61
#define	DIVF	62
#define	MULF	63
#define	CPF	64
#define	NEGF	65
#define	SOB	70
#define	CALL	71
#define	END	72

#define	JEQ	0
#define	JNE	1
#define	JLE	2
#define	JGE	3
#define	JLT	4
#define	JGT	5
#define	JLO	6
#define	JHI	7
#define	JLOS	8
#define	JHIS	9
#define JPL	10
#define JMI	11

#define	BYTE	100
#define LONG	101
#define	LSIZE	512

#define	FREG	5
#define	NREG	9
#define	RT1	2*NREG
#define	RT2	2*NREG+1
#define	LABHS	127
#define	OPHS	95

struct usage {
	char dir, ind;
};

struct node {
	char	op;
	char	subop;
	struct	node	*forw;
	struct	node	*back;
	struct	node	*ref;
	int	labno;
	char	*code;
	int	refc;
#ifndef SMALL
	struct usage dst;
	struct usage src;
#endif
};

struct optab {
	char	*opstring;
	int	opcode;
} optab[];

char	line[LSIZE];
struct	node	first;
char	*curlp;
int	nbrbr;
int	nsaddr;
int	ncomp0;
int	redunm;
int	ndupl;
int	nshortc;
int	iaftbr;
int	njp1;
int	njr;
int	nrlab;
int	nxjump;
int	ncmot;
int	nrevbr;
int	loopiv;
int	nredunj;
int	nskip;
int	ncomj;
int	nsob;
int	nrtst;
int	nlit;
int	nshft;

int	nchange;
int	isn;
int	debug;
int	lastseg;
char	*lasta;
char	*lastr;
char	*firstr;
char	revbr[];
char	regs[2*(NREG+1)][20];
char	conloc[20];
char	conval[20];
char	ccloc[20];

struct optab *ophash[OPHS];
struct	node *nonlab();
char	*copy();
char	*sbrk();
char	*findcon();
struct	node *insertl();
struct	node *codemove();
char	*sbrk();
char	*alloc();
unsigned streval();
