#include "defs.h"
/*	STATUS

	supports Plexus form floating point operations
	nonsense forms (e.g., a JP immediate) are not flagged

	Z8000 NS C -- dissassembler module for pass 3	*/


#include "d4.h"		/* some shared definitions with zp3.c */

static char version [] = { " %W% %G% ui" };

/*	defines for the addressing types
	the mapping is setup so (mode>>1) is a good index
*/

#define	R	0
#define IR	1
#define IM	2
#define	DA	6
#define X	7
#define	RA	8
#define BA	10
#define BX	11


/*	masks to select nibbles 	*/

#define	N1	0170000
#define N2	0007400
#define N3	0000360
#define N4	0000017


/* 	masks to select fields of the format byte 	*/

#define KINDMSK	0160
#define FMTMSK	0017
#define ISLONG	0200

int	pending,	/* input pending? */
	iq[4],		/* storage for instruction words */
	rq[4],		/* storage for relocation constants */
	iqp,		/* next available iq[] slot */
	ifmt,		/* format for this instruction */
	iopx,		/* index into the opcode table */
	imode,		/* mode for this instruction */
	isize,		/* size (words) for this instruction */
	inchack,	/* used to adjust encoding for inc and dec */
	bytreg,		/* flags byte register interpretation */
	twicesize,	/* flag for mult, div, exts */
	onereg,		/* flag indicating one register in operand */
	shift;		/* flag indicating a shift instruction */
int	aflag;
int	zflag;
int	disass;
int	Sflag;
L_INT	localval;
CHAR	*hexint;
POS	*endhdr;
SYMTAB	symbol;
CHAR	*outdefault;
INT	symbins;

char 	*opstr;		/* pointer to opcode string */
long	inkdot();



/*  a table of instruction size by format and addressing mode
    addressing mode encoding is such that this table may be
    indexed by (mode >> 1)
*/


char isiztab [6] [10] = {  /* isiztab[ mode>>1, format-1]  */
/* R, IR	*/	1,1,0,0,2,2,2,1,1,1,
/* IM (W/B)	*/	2,2,1,0,0,2,0,0,1,0,
/* IM (L)	*/	3,3,0,0,0,0,0,0,0,0,
/* DA, X	*/	2,2,0,0,3,3,2,0,1,0,
/* RA		*/	0,0,1,2,0,0,1,0,0,0,
/* BA,BX	*/	0,0,0,2,0,0,0,0,0,0
};


/*	this table contains the reserved words for the ldctl instructions
*/

char *ctlinst[] = {
	"",
	"FLAGS",
	"FCW",
	"REFRESH",
	"PSAPSEG",
	"PSAPOFF",
	"NSPSEG",
	"NSPOFF"
};

/*	this table contains all opcodes.  special characters are 
	interpreted to control the disassembly (in part)
	*** floating operations are handled elsewhere ***
*/

char *optab[] = {
"???",		/* missing opcodes fall here */
"<bit%",	"<res%",	"<set%",	"adc%",		"add%",
"and%",		"bit%",		"call",		"calr",		"clr%",
"com%",		"comflg",	"cp%",		"cpd%", 	"cpdr%",
"cpi%", 	"cpir%",	"cpsd%",	"cpsdr%",	"cpsi%",
"cpsir%",	"d$jnz",	"dab",		"dec%!",	"div@%",
"ex%",		"exts@&",	"inc%!",	"jp",		"jr",
"ld%",		"lda",		"ldb",		"ldctlb|",	"ldi~%",
"ldk",		"ldm!",		"mult@%",	"neg%",		"nop",
"or%",		"pop%",		"push%",	"resflg",	"ret",
"rl%^",		"rlc%^",	"rldb",		"rr%^",		"rrc%^",
"rrdb",		"sbc%",		"sc",		"sdl%>",	"set%",
"setflg",	"sla%>",	"sll%>",	"sll%>",	"sub%",
"tcc%",		"test%",	"trdb",		"trdrb",	"trdrb",
"trib",		"trirb",	"trtdb",	"trtdrb",	"trtib",
"trtirb",	"tset%",	"xor%",		"sda%>",	"ldd~%",
"ld#%",		"lda#",		"iret",		"mset",		"mres",
"mbit",		"mreq",		"ldctl",	"+ini~=",	"+outi~=",
"+in=",		"+out=",	"+ind~=",	"+outd~=",	"in=",
"out="
};


char opmap [] = {
	5,5,60,60,41,41,6,6,73,73,13,13,0,0,0,0,
	13,43,60,43,31,42,5,42,38,38,25,25,0,31,29,8,
	31,31,2,2,3,3,1,1,28,28,24,24,26,26,31,31,
	76,76,76,76,77,76,0,76,0,0,0,0,90,90,91,91,
	5,5,60,60,41,41,6,6,73,73,13,13,0,0,0,0,
	13,43,60,43,31,42,5,42,38,38,25,25,0,31,29,8,
	31,31,2,2,3,3,1,1,28,28,24,24,26,26,31,31,
	31,31,31,31,32,31,32,31,0,0,0,0,0,83,0,53,
	5,5,60,60,41,41,6,6,73,73,13,13,0,0,0,0,
	13,43,60,43,31,42,5,42,38,38,25,25,0,0,45,0,
	31,31,2,2,3,3,1,1,28,28,24,24,26,26,61,61,
	23,27,0,0,4,4,52,52,0,0,0,0,51,36,48,0,
	33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,
	9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
	30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,
	22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22
};




char fmttab[] = {
	 0022,0022,0022,0022,0022,0022,0022,0022
	,0022,0022,0022,0022,0001,0001,0000,0000
	,0222,0322,0222,0122,0222,0302,0222,0102
	,0222,0022,0222,0022,0002,0242,0061,0121
	,0022,0022,0041,0041,0041,0041,0041,0041
	,0041,0041,0041,0041,0022,0022,0042,0042
	,0104,0104,0044,0044,0104,0304,0000,0244
	,0000,0000,0003,0003,0067,0067,0087,0087
	,0022,0022,0022,0022,0022,0022,0022,0022
	,0022,0022,0022,0022,0001,0001,0000,0000
	,0222,0322,0222,0122,0222,0302,0222,0102
	,0222,0022,0222,0022,0002,0242,0061,0121
	,0022,0022,0041,0041,0041,0041,0041,0041
	,0041,0041,0041,0041,0022,0022,0042,0042
	,0064,0064,0024,0024,0064,0264,0022,0224
	,0000,0000,0000,0007,0000,0030,0000,0111
	,0022,0022,0022,0022,0022,0022,0022,0022
	,0022,0022,0022,0022,0001,0001,0072,0000
	,0222,0322,0222,0122,0222,0302,0222,0102
	,0222,0022,0222,0022,0002,0000,0071,0000
	,0022,0022,0041,0041,0041,0041,0041,0041
	,0041,0041,0041,0041,0022,0022,0101,0101
	,0021,0021,0004,0004,0022,0022,0022,0022
	,0005,0000,0006,0006,0022,0041,0022,0000
	,0043,0043,0043,0043,0043,0043,0043,0043
	,0043,0043,0043,0043,0043,0043,0043,0043
	,0103,0103,0103,0103,0103,0103,0103,0103
	,0103,0103,0103,0103,0103,0103,0103,0103
	,0063,0063,0063,0063,0063,0063,0063,0063
	,0063,0063,0063,0063,0063,0063,0063,0063
	,0023,0023,0023,0023,0023,0023,0023,0023
	,0023,0023,0023,0023,0023,0023,0023,0023
	};
/*	The right nibble is the format routine, the middle
	nibble is the case element.
*/

struct aux {
	int pat; char xix, fm; 
};


struct aux aux1[] = {
	0x0c00, 11, 021,
	0x0c01, 13, 025,
	0x0c02, 39, 021,
	0x0c04, 62, 021,
	0x0c05, 31, 025,
	0x0c06, 72, 021,
	0x0c08, 10, 021,
	0x0d00, 11, 021,
	0x0d01, 13, 025,
	0x0d02, 39, 021,
	0x0d04, 62, 021,
	0x0d05, 31, 025,
	0x0d06, 72, 021,
	0x0d08, 10, 021,
	0x0d09, 43, 025,
	0x4c00, 11, 021,
	0x4c01, 13, 025,
	0x4c02, 39, 021,
	0x4c04, 62, 021,
	0x4c05, 31, 025,
	0x4c06, 72, 021,
	0x4c08, 10, 021,
	0x4d00, 11, 021,
	0x4d01, 13, 025,
	0x4d02, 39, 021,
	0x4d04, 62, 021,
	0x4d05, 31, 025,
	0x4d06, 72, 021,
	0x4d08, 10, 021,
	0x8c00, 11, 021,
	0x8c01, 34, 030,
	0x8c02, 39, 021,
	0x8c04, 62, 021,
	0x8c06, 72, 021,
	0x8c08, 10, 021,
	0x8c09, 34, 030,
	0x8d00, 11, 021,
	0x8d01, 56, 031,
	0x8d02, 39, 021,
	0x8d03, 44, 031,
	0x8d04, 62, 021,
	0x8d05, 12, 031,
	0x8d06, 72, 021,
	0x8d07, 40, 051,
	0x8d08, 10, 021,
	0x0000, 00, 000
};


struct aux aux2[] = {
	0x1c01, 37, 026,
	0x1c08, 62, 0221,
	0x1c09, 37, 046,
	0x5c01, 37, 026,
	0x5c08, 62, 0221,
	0x5c09, 37, 046,
	0x9c08, 62, 0221,
	0,0,0
};

struct aux aux3[] = {
	0x3a00, 84, 0x46,
	0x3a01,	84, 0x46,
	0x3a02, 85, 0x46,
	0x3a03, 85, 0x46,
	0x3a04, 86, 0x17,
	0x3a05, 86, 0x17,
	0x3a06, 87, 0x27,
	0x3a07, 87, 0x27,
	0x3a08, 88, 0x46,
	0x3a09, 88, 0x46,
	0x3a0a, 89, 0x46,
	0x3a0b, 89, 0x46,
	0x3b00, 84, 0x46,
	0x3b01, 84, 0x46,
	0x3b02, 85, 0x46,
	0x3b03, 85, 0x46,
	0x3b04, 86, 0x17,
	0x3b05, 86, 0x17,
	0x3b06, 87, 0x27,
	0x3b07, 87, 0x27,
	0x3b08, 88, 0x46,
	0x3b09, 88, 0x46,
	0x3b0a, 89, 0x46,
	0x3b0b, 89, 0x46,
	0,0,0
};



struct aux aux4[] = {
	0xb200, 46, 0x11,
	0xb201, 58, 0x15,
	0xb202, 46, 0x21,
	0xb203, 54, 0x66,
	0xb204, 49, 0x11,
	0xb206, 49, 0x11,
	0xb300, 46, 0x11,
	0xb301, 58, 0x15,
	0xb302, 46, 0x21,
	0xb303, 54, 0x66,
	0xb304, 49, 0x11,
	0xb305, 58, 0x95,
	0xb306, 49, 0x11,
	0xb307, 54, 0xE6,
	0xb208, 47, 0x11,
	0xb209, 57, 0x15,
	0xb20a, 47, 0x11,
	0xb20b, 74, 0x66,
	0xb20c, 50, 0x11,
	0xb20e, 50, 0x11,
	0xb308, 47, 0x11,
	0xb309, 57, 0x15,
	0xb30a, 47, 0x11,
	0xb30b, 74, 0x66,
	0xb30c, 50, 0x11,
	0xb30d, 57, 0x95,
	0xb30e, 50, 0x11,
	0xb30f, 74, 0xE6,
	0,0,0
};


struct aux aux5[] = {
	0xb800, 66, 0144,
	0xb802, 70, 0144,
	0xb804, 71, 0144,
	0xb806, 67, 0144,
	0xb808, 63, 0144,
	0xb80a, 68, 0144,
	0xb80c, 65, 0144,
	0xb80e, 69, 0144,
	0,0,0
};


struct aux aux6[] = {
	0xba00, 16, 0126,
	0xba01, 35, 0126,
	0xba02, 20, 0126,
	0xba04, 17, 0126,
	0xba06, 21, 0126,
	0xbb00, 16, 0126,
	0xbb01, 35, 0106,
	0xbb02, 20, 0126,
	0xbb04, 17, 0126,
	0xbb06, 21, 0126,
	0xba08, 14, 0126,
	0xba09, 75, 0106,
	0xba0a, 18, 0126,
	0xba0c, 15, 0126,
	0xba0e, 19, 0126,
	0xbb08, 14, 0126,
	0xbb09, 75, 0106,
	0xbb0a, 18, 0126,
	0xbb0c, 15, 0126,
	0xbb0e, 19, 0126,
	0,0,0
};


struct aux aux7[] = {
	0x7b00, 78, 0x29,
	0x7b08, 79, 0x29,
	0x7b09, 80, 0x29,
	0x7b0d, 81, 0x29,
	0x7b09, 82, 0x11,
	0,0,0
};


struct aux *auxtab[] = {
	0, &aux1[0], &aux2[0], &aux3[0], &aux4[0],
	&aux5[0], &aux6[0], &aux7[0], 0
};



char *concode[] = {	/* table of condition code vs bitsettings */
	"**", "lt", "le", "ule", "ov", "mi", "eq", "ult",
	"","ge", "gt", "ugt", "nov", "pl", "ne", "uge"
};


char rgnum[] = {   /* maps register numbers for floating point */
	3, 2, 2, 0 
};


char *fops[] = {		/* table of floating point opcodes */
	"?f", "fadd`", "fsub`", "fmul`", "fdiv`", "fcp`", "fneg@`", "?f",
	"?f", "?f", "?f", "?f", "?f", "?f", "?f", "?f"
};

char *fcon[] = {  /* floating conversion routines */
	"fitoi", "fltoi", "fftoi", "fdtoi",
	"fitol", "fltol", "fftol", "fdtol",
	"fitof", "fltof", "fftof", "fdtof",
	"fitod", "fltod", "fftod", "fdtod",
};


/*	this table of function addresses selects the appropriate
	by format tags...
*/

extern	fmterr(), fmt1(), fmt2(), fmt3(), fmt4(), fmt5(), fmt6(), fmt7(),
	fmt8(), fmt9(), fmt10();

int (*tagfmts[]) () = {
	fmterr, fmt1, fmt2, fmt3, fmt4, fmt5, fmt6, fmt7,
	fmt8, fmt9, fmt10
};


extern int zflag;   /* a debug flag for the disassembler */
extern int aflag;	/* output assembled listing */
extern int Sflag;	/* output assembly language */
extern int aaflag;	/* aflag | Sflag */


/*	this is the main driver for the disassembler.  from the 
	leading word of each instruction it computes the format,
	size, and addressing mode.  it then collects as many 
	additional words as it needs and prints the reusult 
	in form.
*/


putdat ( dat , key ) int dat, key; {

	printf(outdefault, dat);
	spaces(12);
	printf(outdefault, dat);
	puttag(key);
}

int dotinc;
long dot;

putins ( ins, idsp , rel ) int ins, idsp, rel; {

	/* save the value and count it */
	iqp = 0;
	rq[iqp] = rel;
	iq[iqp++] = ins;
	dotinc = 2;


	onereg = twicesize = bytreg = inchack = shift = 0;  /* no special cases now */
	imode = amode(ins);
	getfmt(ins); /* sets ifmt and iopx and may fixup imode */
	if((ifmt & ISLONG) && (imode == IM)) imode += 2;
	isize = isiztab [imode>>1] [(ifmt & FMTMSK)-1];
/*
	if(isize == 0) {printf(".word\t"); printf(outdefault,ins); return;}
*/
	pending = isize -1;

	while(pending>0) {
		iq[iqp] = chkget(inkdot(dotinc),idsp);
		rq[iqp++]=0;
		dotinc+=2;
		pending--;
	}

	/* print instruction */
	disassm();
}


/*	---- here follow the support routines in alphabetical order ----   */




amode(ins) int ins; {  /* determine addressing mode */
	switch (ins & 0140000) { /* select on mode bits */

case 000000:
	if((ins & 030000)==030000) {
		if((ins & 0x0c00) == 0x0c00)
			return(RA);
		if(ins & 0360) return (BA); else return (RA);
	} else
		if(ins & 0360) return (IR); else return (IM);

case 040000:
	if((ins & 0xff00) == 0x7d00)	/* ldctl hack */
		return(R);
	if((ins & 0360) != 0) {
		if( (ins & 0x7700) == 0x7600 ) return(X); /* hack for lda! */
		if((ins & 030000)==030000) return (BX);
		return (X);
	}
	return(DA);

case 0100000:
	return (R);

case 0140000:
	return (RA);
	}
}

bmode(ins) int ins; { /* fix up addressing mode for special cases */

	switch (ins&0140000) { /* select on mode bits */
case 0000000:	return(IR);
case 0040000:	return( (ins & 0360)? X : DA); 
case 0100000:	return(R);
	}
}


comma () {
	printc(',');
}


disassm() { 

	int i;

if(zflag)printf("%x found %s  at %d format %x mode %d\n",
		 iq[0],  opstr, iopx , ifmt, imode);

	if (disass) {
		if (outdefault == hexint)
			printf("%-8h",(unsigned) dot);
		else
			printf("%-7o",(unsigned) dot);
		for (i=0; i<iqp; i++) {
			if (outdefault == hexint)
				printf("%-5h",iq[i]);
			else
				printf("%-8o",iq[i]);
		}
		for (; i<3; i++) {
			printf("     ");
			if (outdefault != hexint)
				printf("   ");
		}
		printc(' ');
	}

	if(isize == 0) {printf(".word\t"); printf(outdefault,iq[0]); return;}

	putop(opstr);
	printc('\t');

	(*tagfmts [ifmt & 017] ) ((ifmt>>4)&07);  /* process tags */

 }


getfmt (ins) int ins; {  /* set ifmt and iopx and opstr */
	register int i;
	register struct aux *p;
	i = (ins >> 8) & 0377;  /* high byte of instruction */
	ifmt = fmttab[i];
	if(i == 0x8e) { /* floating point operation */
		if(ins & 0x10) { /*conversion*/
			opstr = fcon[ ins & 0x0f ];
		} else {
			opstr = fops[ ins & 0x0f ];
		}
		return;
	} else
		iopx = opmap[i];
	if(optab[iopx][0] == '>') ifmt = 066;  /* dynamic res, set, bit */
	/* return if not a special case */
	if((ifmt&KINDMSK)!=0){ opstr = optab[iopx];  return;}

	/* well... it's one of the funny cases... */

	p = auxtab[ifmt & FMTMSK];  /* initial pointer */

	/* now search for a match */

	i = ins & 0xff0f;  /* make a key */
	while(p->pat != i && p->pat != 0) p++;
	/* got the darn thing */
	iopx = p->xix; ifmt = p->fm;
	imode = bmode(ins);  /* fix up addressing mode */
	opstr = optab[iopx];
	return;
}


ierror(bad, msg) int bad; char *msg; {
	printf("***** %s ***** %x\n", msg, bad);
	iqp = 0;
}


fmt1( ftag ) int ftag; {
	register int n3, n4, i;
	i = iq[0];
	n3 = (i>>4) &017;
	n4 = i & 017;
	switch (ftag) {
case 1:		prim(imode,n3); return;

case 2:		prim(imode,n3); comma();
		/* inchack adjusts output value from encoded value */
		printc('$');
		putdec(n4 + inchack );   return;

case 3:
case 4:		if( n4 != 8 ) { putcc(n4); comma(); }
case 5:		prim(-imode,n3); return;
	}
}


fmt2( ftag ) int ftag; {
	register int n3, n4, i;
	i = iq[0];
	n3 = (i>>4) & 017;
	n4 = i & 017;

	switch ( ftag ) {

case 1:		puttreg(n4); comma();
		prim(imode,n3); return;

case 2:		prim(imode,n3); comma();
		puttreg(n4); return;

case 3:		prim(imode,n3);
		return;

case 4:		if(imode == DA || imode == X) {
			imode = X;  if(n4 == 0) imode = DA;
		}
		prim(imode,n4); comma();
		prim(IR,n3); return;

case 5:		if(imode == DA || imode == X) {
			imode = X;  if(n4 == 0) imode = DA;
		}
		prim(IR, n3); comma();
		prim(imode,n4); return;

	}
}


fmt3 ( ftag ) int ftag; {
	register int i,j;
	i = iq[0];

	switch(ftag) {

case 1:		puttreg ((i>>8)&017); comma();
		i &= 0177; i += i;
		prim( RA, -i ); return;

case 2:		puttreg((i>>8) & 017); comma();
		printc('$'); putdec( i & 0377);  return;

case 3:		if( ((i>>8)&017) != 8 ) {putcc((i>>8)&017); comma(); }
		i &= 0377;
		i |= -(i & 0200);
		i += i;
		prim( RA, i ); return;

case 4:		i &= 07777;
		i |= -(i & 04000);
		i += i;
		prim( RA, -i ); return;

	}
}


fmt4( ftag ) int ftag; {
	register int i, j;
	i = iq[0]; j = iq[1];

	switch ( ftag ) {

case 1:
case 2:		prim(imode, (i>>4) & 017);
		comma();
		puttreg(i & 017);
		return;

case 3:
case 4:		puttreg (i & 017);
		comma();
		if (((i&0xb000) == 0x3000) && ((i&0x00f0) == 0))
			prim(imode, j);
		else
			prim(imode, (i>>4) & 017);
		return;
	}
}


fmt5 ( ftag ) int ftag; {
	register int i,j;
	i = iq[0]; j = iq[iqp-1];
	iqp--;  /* get rid of the immediate value */
	prim(imode, (i>>4) & 017); comma();
	iqp++;
	prim(IM,j);
	return;
}


fmt6 ( ftag ) int ftag; {
	register int i,j;
	i = iq[0]; j = iq[1];

	switch( ftag ) {

case 1:		prim(R,(j>>8)&017); comma();
		prim(imode,(i>>4)&017); comma();
		printc('$');
		putdec((j&017) + inchack); return;

case 2:		prim(imode,(i>>4)&017); comma();
		prim(R,(j>>8)&017); comma();
		printc('$');
		putdec((j&017) + inchack); return;

case 3:		prim(IR,(j>>8)&017); comma();
		prim(IR, i & 017); return;

case 4:		prim(IR, (j>>4) & 017); comma();
		prim(IR, (i>>4) & 017); comma();
		prim( R, (j>>8) & 017); return;

case 5:		prim(R,(j>>4)&017);  comma();
		prim(IR,(i>>4)&017);  comma();
		prim( R,(j>>8) & 017); comma();
		putcc( j & 017); return;

case 6:		prim(imode,(i>>4)&017); comma();
		prim( R , (j>>8) & 017); return;
	}
}


fmt7 ( ftag ) int ftag; {
	register int i,j;
	i = iq[0]; j = iq[1];

	switch(ftag) {
case 1:		prim( R , (i>>4)&017); comma();
		prim( DA, j); return;

case 2:		prim( DA, j); comma();
		prim( R , (i>>4)&017); return;

case 3:		prim( R , (i&017)); comma();
		prim( IR, (i>>4)&017); return;

case 4:		prim( IR, (i>>4)&017); comma();
		prim( R, (i&017)); return;
	}
}


fmt8 ( ftag ) int ftag; {
int i;
	switch(ftag) {
case 1:		i = iq[0]&0x0f;
		if (i>0x07) {
			printf("%s,",ctlinst[i%8]);
			prim(R,(iq[0]>>4)&017);
			return;
		}
		else {
			prim(R,(iq[0]>>4)&017);
			printc(',');
			printf("%s",ctlinst[i]);
			return;
		}
	}
}


fmt9 ( ftag ) int ftag; {
	switch(ftag) {
case 1:		printf(outdefault, (iq[0]>>4)&017); return;
case 2:		return;
case 3:		if ((iq[0]&017) != 8)
			putcc(iq[0] & 017);
		return;
case 4:		printc('$');
		printf(outdefault, iq[0] & 0377); return;
	}
}


fmt10( ftag ) int ftag; {  /* floating point operations fall here */

	register int i,j,k;
	k = iq[0];  /* get instruction */
	if(k & 0x10 ) { /* conversion */
		i = ((k&0x80)?0:4) + rgnum[(k>>2)&03];
		j = ((k&0x40)?0:4) + rgnum[ k & 03 ];
	} else {
		j = (k & 0x20) ? 1 : 0;
		i = ((k&0x80)?0:4) + rgnum[2+j];
		j = ((k&0x40)?0:4) + rgnum[2+j];
	}
	putfreg(i);
	if (!onereg) {
		comma();
		putfreg(j);
		onereg = 0;
	}
	return;
}


fmterr( ftag ) int ftag; {
	printf("\ndisassembler error\n");
	return;
}

nl() {
	printc('\n');
}


/*	print address by mode
*/

prim ( mode, p1) int mode,p1; {

	int space;
	long val;
	register int j;   j = iqp-1;

	space = DSYM;
	if (mode < 0) {
		space = ISYM;
		mode = -mode;
	}

	switch (mode) {

case IR:
		printc('*');
		putreg(p1);
		return;

case R:		puttreg(p1);
		return;

case IM+2:	/* long immediate */
case IM:	printc('$');
		if((ifmt & ISLONG) && (++shift)) {
			val = *((long *) &iq[j-1]);
			if (outdefault == hexint)
				printf("%X",val);
			else
				printf("%O",val);
		}
		else if (bytreg)
			printf(outdefault,iq[j]&0x00ff);
		else
			putsym(iq[j],space);
		puttag(rq[j]);
		return;

case DA:	putsym(iq[j],space);
		puttag(rq[j]);
		return;

case X:		if( p1 == 14 && symbins ) {
			findsym((int)dot, -ISYM);
			if (*symbol.symc == '~') while( nextlocal() ) {
				if( iq[j] == symbol.symv ) {
					if( (int) symbol.symv > 0 )
						printc('+');
					printf("%s",symbol.symc);
					goto doreg;
				}
			}
		}
		putsym(iq[j],DSYM);
		puttag(rq[j]);
doreg:
		printc('(');
		putreg(p1);
		printc(')');
		return;

case RA:	p1 += inkdot(dotinc);
		putsym(p1,ISYM);
		if (!symbins) {
			printf("+.-");
			printf(outdefault,(int)dot);
		}
		return;

case BA:	putreg(p1);
		printc('(');
		printc('$');
		putsym(iq[j],DSYM);
		puttag(rq[j]);
		printc(')');
		return;

case BX:	putreg(p1);
		printc('(');
		putreg((iq[j]>>8)&017);
		printc(')');
		return;
default:	/* should never happen */
		printf("***mode error %d ***", imode);
		return;

	}
}


putsym(val,space)
int val;
int space;
{

	if (symbins)
		if (((val >= 16) || (val <= -16)) || space == ISYM)
			if (valpr(val,space))
				return;
	putnum(val);
}


putcode() {		/* output code values in hex */
	register int i;

	for(i=0;i<3;i++) {
		if(i<iqp)  printf(outdefault, iq[i]);
		   else    spaces(4);
	}
	spaces(3);
	printc('\t');
}


putcc ( ccval ) int ccval; {
	printf( concode[ ccval ] );
}


putnum(val) {
	if ((val <= 8) && (val >= -8)) {
		printf("%d",val);
		return;
	}
	printf(outdefault,val);
}


putdec( val ) int val; {
	register int c;
	if (c = val/10) putdec(c);
	printc('0' + (val % 10));
}


puthex(val) int val; {
	if(val<0){printc('-'); val = -val;}
	if(val == 0 ) {printc('0'); return; }
	if ((val < 8) && (val > -8)) {
		printf("%d",val);
		return;
	}
	printf(outdefault,val);
}


pthx( val ) int val; {
	register d;
	if(val) pthx( (val>>4) & 0xfff  );
	d = val & 017;
	printc( (d<10)? '0'+d : 'a'-10+d);
}


puthx(val) int val; {
	register int i,v,d;
	if (val == 0) {
		printc('0');
		return;
	}
	if ((val < 10) && (val > -10)) {
		putdec(val);
		return;
	}
	i = 12; v = val;
	while(i>=0) {
		d = (v>>i) & 017;
		printc( (d<10)? '0'+d: 'a'-10+d);
		i -= 4;
	}
}



putop ( os ) char *os; { /* print opstring with interpretation */
	register int c,i; register char *ops;
	ops = os;
	while(c = *opstr++) {
		switch(c){
case('<'):		break;

case('%'):		if (ifmt & ISLONG ) printc('l');
			else
case('='):
			if (!(iq[0]&0400)) {printc('b'); bytreg++; }
			break;

case('$'):		if( !(iq[0] & 0200)) { printc('b'); bytreg++; }
			break;

case('&'):		if((iq[0]&N4)==0) { printc('b'); bytreg++; }  
			else
			if((iq[0]&N4)==7) printc('l');
			break;

case('#'):		if((iq[0]&N3)==0) printc('r');
			break;

case('^'):		if(iq[0]&02) iq[0] &= 0177762; /* src = 2 */
			else  iq[0] = (iq[0] & 0177760) | 01; /* src = 1 */
			break;

case('!'):		inchack++;
			break;

case('~'):		/* repeat ? */
			i = iq[1] & 017;
			if( i == 0 ) printc('r');
			break;

case('+'):		if (iq[0] & 1) printc('s');
			break;

case('`'):		if(iq[0] & 040) printc('d');
			break;

case('@'):		twicesize++;
			break;

case('1'):		onereg++;
			break;

case('>'):		shift = -2;
			break;

case('|'):		bytreg++;
			break;

default:		printc(c); 
			break;
		} /* end of switch*/
	} /* end of while */
}


puttag ( tag ) int tag; { /* print relocation word stuff */

	register int j;
	switch ( tag & 017 ) {

case TXTREF:	printf("(TXT)"); return;

case DATREF:	if(tag == DATREF + DATFLAG) printf("(DAT)");
			else printf("(CON)");
		return;

case BSSREF:	printf("(BSS)"); return;

case EXTREF:	j = (tag>>4) & 07777;
		printc('+'); 
		return;

	}
}


putreg( val ) int val; {
	if( symbins && val >= 8 && val <= 13) {
		findsym((int)dot, -ISYM);
		if( *symbol.symc == '~' ) while( nextlocal() )
			if( symbol.symf == 024 && val == symbol.symv ) {
				printf("%s",symbol.symc);
				return;
			}
	}
	printc('r');
	putdec(val);
}


putfreg( val ) int val; {
	printc('r');
	switch( val % 4 ) {
		case 0: printc('q'); break;
		case 2: printc('r'); break;
	}
	putdec(val);
}


puttreg( val ) int val; {  /* put typed register designator */

	if( bytreg ) {
		if (twicesize)
			putreg(val);
		else {
			printc('r');
			printc( (val & 0x08 )?'l':'h' );
			putdec( val & 07 );
		}
		twicesize = 0;
		return;
	}
	if( ifmt & ISLONG ) {
		if (twicesize) {
			printc('r');
			printc('q');
			putdec(val & 0xf);
			twicesize = 0;
			return;
		}
		if (++shift)
			printc('r');
	}
	if (twicesize)
		printc('r');
	putreg(val);
	twicesize = 0;
}


spaces ( count ) int count; {  /* space output file */
	register int c;
	c = count;
	while(c--) printc(' ');
}


