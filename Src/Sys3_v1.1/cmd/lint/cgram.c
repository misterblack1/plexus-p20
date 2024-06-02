# define NAME 2
# define STRING 3
# define ICON 4
# define FCON 5
# define PLUS 6
# define MINUS 8
# define MUL 11
# define AND 14
# define OR 17
# define ER 19
# define QUEST 21
# define COLON 22
# define ANDAND 23
# define OROR 24
# define ASOP 25
# define RELOP 26
# define EQUOP 27
# define DIVOP 28
# define SHIFTOP 29
# define INCOP 30
# define UNOP 31
# define STROP 32
# define TYPE 33
# define CLASS 34
# define STRUCT 35
# define RETURN 36
# define GOTO 37
# define IF 38
# define ELSE 39
# define SWITCH 40
# define BREAK 41
# define CONTINUE 42
# define WHILE 43
# define DO 44
# define FOR 45
# define DEFAULT 46
# define CASE 47
# define SIZEOF 48
# define ENUM 49
# define LP 50
# define RP 51
# define LC 52
# define RC 53
# define LB 54
# define RB 55
# define CM 56
# define SM 57
# define ASSIGN 58

# line 108 "mip/cgram.y"
# include "mfile1"
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;

# line 127 "mip/cgram.y"
	static int fake = 0;
	static char fakename[NCHNAM+1];
# define YYERRCODE 256

# line 785 "mip/cgram.y"


NODE *
mkty( t, d, s ) unsigned t; {
	return( block( TYPE, NIL, NIL, t, d, s ) );
	}

NODE *
bdty( op, p, v ) NODE *p; {
	register NODE *q;

	q = block( op, p, NIL, INT, 0, INT );

	switch( op ){

	case UNARY MUL:
	case UNARY CALL:
		break;

	case LB:
		q->right = bcon(v);
		break;

	case NAME:
		q->rval = v;
		break;

	default:
		cerror( "bad bdty" );
		}

	return( q );
	}

dstash( n ) OFFSZ n; { /* put n into the dimension table */
	if( curdim >= DIMTABSZ-1 ){
		cerror( "dimension table overflow");
		}
	dimtab[ curdim++ ] = n;
	}

savebc() {
	if( psavbc > & asavbc[BCSZ-4 ] ){
		cerror( "whiles, fors, etc. too deeply nested");
		}
	*psavbc++ = brklab;
	*psavbc++ = contlab;
	*psavbc++ = flostat;
	*psavbc++ = swx;
	flostat = 0;
	}

resetbc(mask){

	swx = *--psavbc;
	flostat = *--psavbc | (flostat&mask);
	contlab = *--psavbc;
	brklab = *--psavbc;

	}

addcase(p) NODE *p; { /* add case to switch */

	p = optim( p );  /* change enum to ints */
	if( p->op != ICON ){
		uerror( "non-constant case expression");
		return;
		}
	if( swp == swtab ){
		uerror( "case not in switch");
		return;
		}
	if( swp >= &swtab[SWITSZ] ){
		cerror( "switch table overflow");
		}
	swp->sval = p->lval;
	deflab( swp->slab = getlab() );
	++swp;
	tfree(p);
	}

adddef(){ /* add default case to switch */
	if( swtab[swx].slab >= 0 ){
		uerror( "duplicate default in switch");
		return;
		}
	if( swp == swtab ){
		uerror( "default not inside switch");
		return;
		}
	deflab( swtab[swx].slab = getlab() );
	}

swstart(){
	/* begin a switch block */
	if( swp >= &swtab[SWITSZ] ){
		cerror( "switch table overflow");
		}
	swx = swp - swtab;
	swp->slab = -1;
	++swp;
	}

swend(){ /* end a switch block */

	register struct sw *swbeg, *p, *q, *r, *r1;
	CONSZ temp;
	int tempi;

	swbeg = &swtab[swx+1];

	/* sort */

	r1 = swbeg;
	r = swp-1;

	while( swbeg < r ){
		/* bubble largest to end */
		for( q=swbeg; q<r; ++q ){
			if( q->sval > (q+1)->sval ){
				/* swap */
				r1 = q+1;
				temp = q->sval;
				q->sval = r1->sval;
				r1->sval = temp;
				tempi = q->slab;
				q->slab = r1->slab;
				r1->slab = tempi;
				}
			}
		r = r1;
		r1 = swbeg;
		}

	/* it is now sorted */

	for( p = swbeg+1; p<swp; ++p ){
		if( p->sval == (p-1)->sval ){
			uerror( "duplicate case in switch, %d", tempi=p->sval );
			return;
			}
		}

	genswitch( swbeg-1, swp-swbeg );
	swp = swbeg-1;
	}
short yyexca[] ={
-1, 1,
	0, -1,
	2, 21,
	11, 21,
	50, 21,
	57, 21,
	-2, 0,
-1, 19,
	56, 78,
	57, 78,
	-2, 7,
-1, 24,
	56, 77,
	57, 77,
	-2, 75,
-1, 32,
	52, 43,
	-2, 41,
-1, 34,
	52, 35,
	-2, 33,
	};
# define YYNPROD 179
# define YYLAST 1170
short yyact[]={

 229,  24, 263, 249,  87,  85,  86,  26,  26,  78,
 227, 128,  76,  26,  75,  77,  21,  21,  10,   9,
  14,  96,  21,  40,  42, 127,  26,  94,  94,  98,
  91,  80,  79,  18,  16,  21,  90, 239, 240, 244,
  64, 248, 237, 238, 245, 246, 247, 251, 250,  81,
  73,  82, 194, 106, 228,  22,  22,  93, 241, 143,
  26,  22, 160,  10,   9,  14, 108,  57, 225,  21,
 230,  54, 272,  20,  22, 129, 271,   8,  50,  16,
  92,  17, 106, 144, 148,  28,  19,  36, 257,  72,
 304, 136, 137, 138, 139, 140, 141,   6, 152, 203,
  36,  35,  41,  43, 303, 298,  93, 287,  22,  55,
 267, 266, 162, 163, 164, 166, 168, 170, 172, 174,
 175, 177, 179, 181, 182, 183, 184, 185, 132, 161,
 134, 129, 101,  99, 189, 154,  89, 198, 187, 157,
 100, 159, 197, 144, 152,  68,  97, 281, 135, 186,
  69, 254, 133,  38,  67, 256, 188,  39, 219, 104,
 145, 151, 110, 107,  71, 156, 204,  55, 205,  38,
 206,  47, 207,  39, 208,  48, 153, 209,  63, 210,
 219, 211, 253, 249,  87,  85,  86, 213, 129,  78,
 155, 150,  76,  33, 200,  77,  47,  66, 223, 214,
  48,  93, 191,  46,  47,  26, 293, 218,  48,  31,
 215,  80,  79, 252,  21, 285, 196, 239, 240, 244,
 145, 248, 237, 238, 245, 246, 247, 251, 250,  81,
 224,  82, 218, 106, 196, 226, 264, 278, 241, 217,
 269,   4, 259, 260, 261, 262, 192, 265, 221, 222,
  70, 279,  45,  22, 273, 195, 201, 242,  62, 294,
 255,  95,  95, 282, 277, 149, 276,  27,  10,   9,
  14, 275, 274, 195, 220, 288, 289, 264, 291, 290,
  27, 149, 100,  51,  16, 284, 105, 283,   9, 114,
  52, 115,  30, 280, 117, 300, 295, 119, 270, 264,
 120, 301, 121,  10, 124,  14, 122, 123, 125, 111,
 118, 113, 116, 114,  27, 115, 190,   7, 117,  16,
 117,  65,  58, 109,  34, 114,  29, 115,  32, 243,
 117, 103, 130, 119, 302, 113, 120, 113, 121, 112,
 124, 126, 122, 123, 125, 111, 118, 113, 116,  49,
  25,  59,  44, 199,  88,  53,  56, 158, 202, 102,
  61, 114,  60, 115,  37,   3, 117,   2, 147, 119,
 299,  83, 120,  11, 121, 112, 124, 126, 122, 123,
 125, 111, 118, 113, 116,  12,   5,  23,  13,  15,
 236, 234, 235, 233, 231, 232,   1, 114,   0, 115,
   0,   0, 117,   0,   0, 119, 297,   0, 120,   0,
 121, 112, 124, 126, 122, 123, 125, 111, 118, 113,
 116,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 114,   0, 115,   0,   0, 117,   0, 242,   0,   0,
   0,   0, 296, 114,   0, 115,   0, 112, 117, 126,
 111, 119, 113, 116, 120,   0, 121,   0, 124, 292,
 122, 123, 125, 111, 118, 113, 116, 114,   0, 115,
   0,   0, 117,   0,   0, 119,   0,   0, 120,   0,
 121,   0, 124,   0, 122, 123, 125, 111, 118, 113,
 116,   0, 114, 112, 115, 126,   0, 117,   0, 114,
 119, 115,   0, 120, 117, 121,   0, 124,   0, 122,
 123, 125, 111, 118, 113, 116,   0, 112, 286, 126,
   0, 113, 116,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 112, 258, 126, 114,   0, 115,   0,   0,
 117,   0,   0, 119,   0,   0, 120,   0, 121,   0,
 124,   0, 122, 123, 125, 111, 118, 113, 116, 114,
   0, 115,   0,   0, 117,   0,   0, 119,   0,   0,
 120,   0, 121,   0, 124, 212, 122, 123, 125, 111,
 118, 113, 116,   0, 216, 112,   0, 126,   0,   0,
   0,   0,   0,   0,   0, 114,   0, 115,   0, 114,
 117, 115,   0, 119, 117,   0, 120, 119, 121, 112,
 124, 126, 122, 123, 125, 111, 118, 113, 116, 111,
 118, 113, 116,   0,   0,   0,   0,   0,   0,   0,
   0, 114,   0, 115,   0,   0, 117,   0,   0, 119,
 193,   0, 120,   0, 121, 112, 124, 126, 122, 123,
 125, 111, 118, 113, 116, 114,   0, 115,   0,   0,
 117,   0,   0, 119,   0,   0, 120,   0, 121,   0,
 124,   0, 122, 123, 125, 111, 118, 113, 116,   0,
   0, 112,   0, 126,  84,  87,  85,  86,   0,   0,
  78,   0,   0,  76,   0,   0,  77,  84,  87,  85,
  86,   0,   0,  78,   0,   0,  76, 126,   0,  77,
   0,   0,  80,  79,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,  80,  79,   0,   0,   0,
  81,   0,  82,   0,   0,  84,  87,  85,  86,   0,
 180,  78,   0,  81,  76,  82,   0,  77,  84,  87,
  85,  86,   0, 178,  78,   0,   0,  76,   0,   0,
  77,   0,   0,  80,  79,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,  80,  79,   0,   0,
   0,  81,   0,  82,   0,   0,  84,  87,  85,  86,
   0, 176,  78,   0,  81,  76,  82,   0,  77,  84,
  87,  85,  86,   0, 173,  78,   0,   0,  76,   0,
   0,  77,   0,   0,  80,  79,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  80,  79,   0,
   0,   0,  81,   0,  82,   0,   0,  84,  87,  85,
  86,   0, 171,  78,   0,  81,  76,  82,   0,  77,
  84,  87,  85,  86,   0, 169,  78,   0,   0,  76,
   0,   0,  77,   0,   0,  80,  79,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  80,  79,
   0,   0,   0,  81,   0,  82,   0,   0,  84,  87,
  85,  86,   0, 167,  78,   0,  81,  76,  82,   0,
  77,  84,  87,  85,  86,   0, 165,  78,   0,   0,
  76,   0,   0,  77,   0,   0,  80,  79, 114,   0,
 115,   0,   0, 117,   0,   0, 119,   0,   0,  80,
  79, 121,   0,   0,  81,   0,  82,   0, 111, 118,
 113, 116,   0, 268,   0,   0,   0,  81,   0,  82,
   0, 131,  84,  87,  85,  86,   0,   0,  78,   0,
   0,  76,   0,   0,  77,  84,  87,  85,  86,   0,
   0,  78,   0,   0,  76,   0,   0,  77,   0,   0,
  80,  79,   0,  10,   0,  14,   0,   0,   0,   0,
   0,   0,   0,  80,  79,   0,   0,   0,  81,  16,
  82,   0,   0,   0,   0,  84,  87,  85,  86,   0,
   0,  81,   0,  82,   0,  74,  84,  87,  85,  86,
   0,   0,  78,   0,   0,  76,   0,   0,  77,  84,
  87,  85,  86,  80,  79,  78,   0,   0,  76,   0,
   0,  77,   0,   0,  80,  79,   0,   0,   0,   0,
   0,  81,   0,  82,   0,   0,   0,  80,  79,   0,
   0, 114,  81, 115,  82, 146, 117,  84,  87,  85,
  86,   0,   0,  78,   0,  81,  76,  82,   0,  77,
   0, 111, 118, 113, 116,   0, 114,   0, 115,   0,
   0, 117,   0,   0, 119,  80,  79, 120,   0, 121,
   0, 124,   0, 122, 123,   0, 111, 118, 113, 116,
   0,   0, 114,  81, 115, 142,   0, 117,   0,   0,
 119,   0,   0, 120,   0, 121,   0,   0,   0, 122,
   0,   0, 111, 118, 113, 116, 114,   0, 115,   0,
   0, 117,   0,   0, 119,   0,   0, 120,   0, 121,
   0,   0,   0,   0,   0,   0, 111, 118, 113, 116 };
short yypact[]={

-1000, -15,-1000,-1000,-1000,  24,-1000, 270, 254,-1000,
 259,-1000,-1000, 157, 326, 141, 322,-1000,  44, 119,
-1000, 203, 203, 201, 121,  20, 233,-1000,-1000,-1000,
 257, 270,-1000, 320,-1000,-1000,-1000,-1000, 207, 123,
 121, 119, 146, 103,  94,-1000,-1000, 199, 109, 973,
-1000,-1000,-1000,  79,-1000,   6,  90,-1000, -29,  58,
-1000,  30,-1000,-1000, 108,1037,-1000,-1000,-1000, 321,
-1000,-1000, 107, 659, 909,  98,1037,1037,1037,1037,
1037,1075, 960,1024, 231,-1000,-1000,-1000, 138, 270,
  88,-1000, 119, 154,-1000,-1000, 137, 320,-1000,-1000,
 119,-1000,-1000,-1000,   5,  72,-1000,-1000, 659,-1000,
-1000,1037,1037, 858, 845, 807, 794, 756,1037, 743,
 705, 692,1037,1037,1037,1037,1037,  82,-1000, 659,
 909,-1000,-1000,1037, 314,-1000,  98,  98,  98,  98,
  98,  98, 960, 195, 599, 205,-1000,  86, 659,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000, 235,  42,
-1000,-1000, 493, 659,-1000,1037, 309,1037, 309,1037,
 307,1037,-1000,1037, 424,1065,1037, 922,1037, 603,
1037,1140,1116, 563, 659, 659, 134, 909,  82, 539,
-1000, 188,1037,-1000, 126, 223, 205,1037,-1000,   6,
-1000,  11,   1,-1000, 659, 659, 659, 659, 659, 659,
 659, 659,1037,-1000,-1000, 129,-1000,1013,  98,  96,
-1000, 104, 126, 659,-1000,-1000,  31,-1000,-1000, 486,
-1000, 181, 181, 181, 181,1037, 181,  54,  53, 896,
 296,-1000,  19, 181, 222, 221,-1000, 216, 214, 215,
1037, 271,1090,-1000,-1000,  92, 213,-1000,-1000, 248,
-1000,-1000, 242, 164, 635,-1000,-1000,-1000,-1000, 461,
  50,-1000,-1000,-1000,1037,1037,1037,1037,-1000, 437,
-1000,-1000, 155,-1000, 209, 181,-1000,-1000, 391, 355,
  48, 319,-1000,-1000,1037,-1000,-1000,-1000,1037,-1000,
 283,  47,  33,-1000,-1000 };
short yypgo[]={

   0, 396,  40, 395, 394, 393, 392, 391, 390, 389,
 388, 387,   0,   2,  14,  97, 386,  77, 385, 373,
  59,  52, 371,  30,  80,   1, 368, 367, 365,  33,
 364, 362, 360,  70, 359, 358,  10, 357,  36, 317,
 356,  21,  67, 355, 354,  71, 353, 352,  73, 351,
 350, 349,  25,  11, 332, 331, 329, 321 };
short yyr1[]={

   0,   1,   1,  27,  27,  28,  28,  30,  28,  31,
  32,  32,  35,  35,  37,  37,  37,  34,  34,  34,
  16,  16,  15,  15,  15,  15,  39,  17,  17,  17,
  17,  17,  18,  18,   9,   9,  40,  40,  42,  42,
  19,  19,  10,  10,  43,  43,  45,  45,  38,  46,
  38,  23,  23,  23,  23,  23,  25,  25,  25,  25,
  25,  25,  24,  24,  24,  24,  24,  24,  24,  11,
  47,  47,  29,  49,  29,  50,  50,  48,  48,  48,
  48,  52,  52,  53,  53,  41,  41,  44,  44,  51,
  51,  54,  33,  55,  36,  36,  36,  36,  36,  36,
  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
  36,  56,  56,  56,   7,   4,   3,   5,   6,   8,
  57,   2,  13,  13,  26,  26,  12,  12,  12,  12,
  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,
  12,  12,  12,  12,  12,  12,  12,  12,  12,  12,
  12,  14,  14,  14,  14,  14,  14,  14,  14,  14,
  14,  14,  14,  14,  14,  14,  14,  14,  14,  20,
  21,  21,  21,  21,  21,  21,  21,  22,  22 };
short yyr2[]={

   0,   2,   0,   1,   1,   2,   3,   0,   4,   2,
   2,   0,   2,   0,   3,   4,   0,   3,   2,   2,
   1,   0,   2,   2,   1,   1,   1,   1,   2,   3,
   1,   1,   5,   2,   1,   2,   1,   3,   1,   3,
   5,   2,   1,   2,   1,   3,   2,   1,   1,   0,
   4,   1,   1,   3,   2,   1,   2,   3,   3,   4,
   1,   3,   2,   3,   3,   4,   3,   3,   2,   2,
   1,   3,   1,   0,   4,   1,   1,   1,   1,   3,
   6,   1,   3,   1,   4,   0,   1,   0,   1,   0,
   1,   1,   4,   1,   2,   1,   2,   2,   2,   7,
   4,   2,   2,   2,   2,   3,   3,   1,   2,   2,
   2,   2,   3,   2,   1,   4,   3,   4,   6,   4,
   0,   2,   1,   0,   1,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   4,
   4,   4,   4,   4,   4,   4,   4,   5,   3,   3,
   1,   2,   2,   2,   2,   2,   2,   2,   4,   4,
   4,   2,   3,   3,   1,   1,   1,   1,   3,   2,
   0,   2,   5,   2,   3,   4,   3,   2,   2 };
short yychk[]={

-1000,  -1, -27, -28, 256, -16, -15, -39, -17,  34,
  33, -19, -18, -10,  35,  -9,  49,  57, -29, -24,
 -48,  11,  50, -11, -25, -50,   2, 256, -17, -39,
  33,  52,   2,  52,   2,  57,  56, -30,  50,  54,
 -25, -24, -25, -24, -47,  51,   2,  50,  54, -51,
  58,  50,  33, -43, -45, -17, -40, -42,   2, -49,
 -31, -32,  51,  55,  -2, -57,  51,  51,  51,  56,
  51,  55,  -2, -12,  52, -14,  11,  14,   8,  31,
  30,  48,  50, -22,   2,   4,   5,   3, -44,  57,
 -38, -23, -24, -25,  22, 256, -41,  56,  58, -48,
 -24, -33, -34, -55, -15, 256,  52,  55, -12,   2,
  55,  26,  56,  28,   6,   8,  29,  11,  27,  14,
  17,  19,  23,  24,  21,  25,  58, -52, -53, -12,
 -54,  52,  30,  54,  32,  50, -14, -14, -14, -14,
 -14, -14,  50, -20, -12, -17,  51, -26, -12,  50,
  53, -45,  56,  22,  -2,  53, -42,  -2, -37, -38,
  57,  57, -12, -12, -12,  58, -12,  58, -12,  58,
 -12,  58, -12,  58, -12, -12,  58, -12,  58, -12,
  58, -12, -12, -12, -12, -12, -41,  56, -52, -12,
   2, -20,  51,  51, -21,  50,  11,  56,  51, -46,
  -2, -15, -35,  57, -12, -12, -12, -12, -12, -12,
 -12, -12,  22,  53, -53, -41,  55,  51, -14,  54,
  51, -21, -21, -12, -23,  57, -29, -36,  53, -12,
 -33,  -4,  -3,  -5,  -7,  -6,  -8,  41,  42,  36,
  37,  57, 256, -56,  38,  43,  44,  45,  40,   2,
  47,  46, -12,  53,  55,  -2,  51,  57,  57, -36,
 -36, -36, -36, -13, -12, -36,  57,  57,  57, -12,
   2,  57,  53, -36,  50,  50,  50,  50,  22, -12,
  22,  55,  50,  39,  43,  51,  57,  57, -12, -12,
 -13, -12,  22,  51,  50, -36,  51,  51,  57,  51,
 -12, -13,  51,  57,  57 };
short yydef[]={

   2,  -2,   1,   3,   4,   0,  20,  24,  25,  26,
  27,  30,  31,   0,  42,   0,  34,   5,   0,  -2,
  72,   0,   0,   0,  -2,  89,  60,  76,  22,  23,
  28,   0,  -2,   0,  -2,   6,  73,  11,   0, 120,
  56,  62,   0,   0,   0,  68,  70,   0, 120,   0,
  90,  69,  29,  87,  44,  47,  85,  36,  38,   0,
   8,   0,  63,  64,   0,   0,  61,  66,  67,   0,
  57,  58,   0,  79,   0, 150,   0,   0,   0,   0,
   0,   0,   0,   0, 164, 165, 166, 167,   0,  88,
  46,  48,  51,  52, 120,  55,   0,  86, 120,  74,
  78,   9,  10,  16,   0,   0,  93,  65, 121,  71,
  59,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  85,  81,  83,
   0,  91, 151,   0,   0, 178, 152, 153, 154, 155,
 156, 157,   0,   0,   0, 170, 161,   0, 124, 177,
  40,  45,  49, 120,  54,  32,  37,  39,  13,   0,
  18,  19, 126, 127, 128,   0, 129,   0, 130,   0,
 131,   0, 132,   0, 133, 134,   0, 135,   0, 136,
   0, 137, 138,   0, 148, 149,   0,  86,  85,   0,
 163,   0,   0, 168, 169, 170, 170,   0, 162,   0,
  53,   0,   0,  17, 140, 141, 142, 143, 139, 144,
 145, 146,   0,  80,  82,   0, 160, 159, 158, 120,
 171,   0, 173, 125,  50,  14,   0,  12,  92,   0,
  95,   0,   0,   0,   0, 123,   0,   0,   0,   0,
   0, 107,   0,   0,   0,   0, 114,   0,   0, 164,
   0,   0, 147,  84, 174,   0, 176,  15,  94,  96,
  97,  98,   0,   0, 122, 101, 102, 103, 104,   0,
   0, 108, 109, 110,   0,   0, 123,   0, 111,   0,
 113, 175,   0, 116,   0,   0, 105, 106,   0,   0,
   0,   0, 112, 172,   0, 100, 115, 117, 123, 119,
   0,   0,   0, 118,  99 };
#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

int yydebug = 0; /* 1 for debugging */
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

			if( yydebug ) printf( "error recovery discards char %d\n", yychar );

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

		if( yydebug ) printf("reduce %d\n",yyn);
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 2:
# line 133 "mip/cgram.y"
ftnend(); break;
case 3:
# line 136 "mip/cgram.y"
{ curclass = SNULL;  blevel = 0; } break;
case 4:
# line 138 "mip/cgram.y"
{ curclass = SNULL;  blevel = 0; } break;
case 5:
# line 142 "mip/cgram.y"
{  yypvt[-1].nodep->op = FREE; } break;
case 6:
# line 144 "mip/cgram.y"
{  yypvt[-2].nodep->op = FREE; } break;
case 7:
# line 145 "mip/cgram.y"
{
				defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass==STATIC?STATIC:EXTDEF );
				} break;
case 8:
# line 148 "mip/cgram.y"
{  
			    if( blevel ) cerror( "function level error" );
			    if( reached ) retstat |= NRETVAL; 
			    yypvt[-3].nodep->op = FREE;
			    ftnend();
			    } break;
case 11:
# line 159 "mip/cgram.y"
{  blevel = 1; } break;
case 13:
# line 164 "mip/cgram.y"
{  bccode();
			    locctr(PROG);
			    } break;
case 14:
# line 170 "mip/cgram.y"
{  yypvt[-1].nodep->op = FREE; } break;
case 15:
# line 172 "mip/cgram.y"
{  yypvt[-2].nodep->op = FREE; } break;
case 17:
# line 176 "mip/cgram.y"
{ curclass = SNULL;  yypvt[-2].nodep->op = FREE; } break;
case 18:
# line 178 "mip/cgram.y"
{ curclass = SNULL;  yypvt[-1].nodep->op = FREE; } break;
case 19:
# line 180 "mip/cgram.y"
{  curclass = SNULL; } break;
case 21:
# line 184 "mip/cgram.y"
{  yyval.nodep = mkty(INT,0,INT);  curclass = SNULL; } break;
case 22:
# line 187 "mip/cgram.y"
{  yyval.nodep = yypvt[-0].nodep; } break;
case 24:
# line 190 "mip/cgram.y"
{  yyval.nodep = mkty(INT,0,INT); } break;
case 25:
# line 192 "mip/cgram.y"
{ curclass = SNULL ; } break;
case 26:
# line 197 "mip/cgram.y"
{  curclass = yypvt[-0].intval; } break;
case 28:
# line 202 "mip/cgram.y"
{  yypvt[-1].nodep->type = types( yypvt[-1].nodep->type, yypvt[-0].nodep->type, UNDEF );
			    yypvt[-0].nodep->op = FREE;
			    } break;
case 29:
# line 206 "mip/cgram.y"
{  yypvt[-2].nodep->type = types( yypvt[-2].nodep->type, yypvt[-1].nodep->type, yypvt[-0].nodep->type );
			    yypvt[-1].nodep->op = yypvt[-0].nodep->op = FREE;
			    } break;
case 32:
# line 214 "mip/cgram.y"
{ yyval.nodep = dclstruct(yypvt[-4].intval); } break;
case 33:
# line 216 "mip/cgram.y"
{  yyval.nodep = rstruct(yypvt[-0].intval,0);  stwart = instruct; } break;
case 34:
# line 220 "mip/cgram.y"
{  yyval.intval = bstruct(-1,0); } break;
case 35:
# line 222 "mip/cgram.y"
{  yyval.intval = bstruct(yypvt[-0].intval,0); } break;
case 38:
# line 230 "mip/cgram.y"
{  moedef( yypvt[-0].intval ); } break;
case 39:
# line 232 "mip/cgram.y"
{  strucoff = yypvt[-0].intval;  moedef( yypvt[-2].intval ); } break;
case 40:
# line 236 "mip/cgram.y"
{ yyval.nodep = dclstruct(yypvt[-4].intval);  } break;
case 41:
# line 238 "mip/cgram.y"
{  yyval.nodep = rstruct(yypvt[-0].intval,yypvt[-1].intval); } break;
case 42:
# line 242 "mip/cgram.y"
{  yyval.intval = bstruct(-1,yypvt[-0].intval);  stwart=0; } break;
case 43:
# line 244 "mip/cgram.y"
{  yyval.intval = bstruct(yypvt[-0].intval,yypvt[-1].intval);  stwart=0;  } break;
case 46:
# line 252 "mip/cgram.y"
{ curclass = SNULL;  stwart=0; yypvt[-1].nodep->op = FREE; } break;
case 47:
# line 254 "mip/cgram.y"
{  if( curclass != MOU ){
				curclass = SNULL;
				}
			    else {
				sprintf( fakename, "$%dFAKE", fake++ );
				defid( tymerge(yypvt[-0].nodep, bdty(NAME,NIL,lookup( fakename, SMOS ))), curclass );
				}
			    stwart = 0;
			    yypvt[-0].nodep->op = FREE;
			    } break;
case 48:
# line 268 "mip/cgram.y"
{ defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass);  stwart = instruct; } break;
case 49:
# line 269 "mip/cgram.y"
{yyval.nodep=yypvt[-2].nodep;} break;
case 50:
# line 270 "mip/cgram.y"
{ defid( tymerge(yypvt[-4].nodep,yypvt[-0].nodep), curclass);  stwart = instruct; } break;
case 53:
# line 276 "mip/cgram.y"
{  if( !(instruct&INSTRUCT) ) uerror( "field outside of structure" );
			    if( yypvt[-0].intval<0 || yypvt[-0].intval >= FIELD ){
				uerror( "illegal field size" );
				yypvt[-0].intval = 1;
				}
			    defid( tymerge(yypvt[-3].nodep,yypvt[-2].nodep), FIELD|yypvt[-0].intval );
			    yyval.nodep = NIL;
			    } break;
case 54:
# line 286 "mip/cgram.y"
{  if( !(instruct&INSTRUCT) ) uerror( "field outside of structure" );
			    falloc( stab, yypvt[-0].intval, -1, yypvt[-2].nodep );  /* alignment or hole */
			    yyval.nodep = NIL;
			    } break;
case 55:
# line 291 "mip/cgram.y"
{  yyval.nodep = NIL; } break;
case 56:
# line 296 "mip/cgram.y"
{  umul:
				yyval.nodep = bdty( UNARY MUL, yypvt[-0].nodep, 0 ); } break;
case 57:
# line 299 "mip/cgram.y"
{  uftn:
				yyval.nodep = bdty( UNARY CALL, yypvt[-2].nodep, 0 );  } break;
case 58:
# line 302 "mip/cgram.y"
{  uary:
				yyval.nodep = bdty( LB, yypvt[-2].nodep, 0 );  } break;
case 59:
# line 305 "mip/cgram.y"
{  bary:
				if( (int)yypvt[-1].intval <= 0 ) werror( "zero or negative subscript" );
				yyval.nodep = bdty( LB, yypvt[-3].nodep, yypvt[-1].intval );  } break;
case 60:
# line 309 "mip/cgram.y"
{  yyval.nodep = bdty( NAME, NIL, yypvt[-0].intval );  } break;
case 61:
# line 311 "mip/cgram.y"
{ yyval.nodep=yypvt[-1].nodep; } break;
case 62:
# line 314 "mip/cgram.y"
{  goto umul; } break;
case 63:
# line 316 "mip/cgram.y"
{  goto uftn; } break;
case 64:
# line 318 "mip/cgram.y"
{  goto uary; } break;
case 65:
# line 320 "mip/cgram.y"
{  goto bary; } break;
case 66:
# line 322 "mip/cgram.y"
{ yyval.nodep = yypvt[-1].nodep; } break;
case 67:
# line 324 "mip/cgram.y"
{
				if( blevel!=0 ) uerror("function declaration in bad context");
				yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,yypvt[-2].intval), 0 );
				stwart = 0;
				} break;
case 68:
# line 330 "mip/cgram.y"
{
				yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,yypvt[-1].intval), 0 );
				stwart = 0;
				} break;
case 69:
# line 337 "mip/cgram.y"
{
				/* turn off typedefs for argument names */
				stwart = SEENAME;
				} break;
case 70:
# line 344 "mip/cgram.y"
{ ftnarg( yypvt[-0].intval );  stwart = SEENAME; } break;
case 71:
# line 346 "mip/cgram.y"
{ ftnarg( yypvt[-0].intval );  stwart = SEENAME; } break;
case 73:
# line 351 "mip/cgram.y"
{yyval.nodep=yypvt[-2].nodep;} break;
case 75:
# line 355 "mip/cgram.y"
{  defid( yypvt[-0].nodep = tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass);
			    beginit(yypvt[-0].nodep->rval);
			    } break;
case 77:
# line 362 "mip/cgram.y"
{  nidcl( tymerge(yypvt[-1].nodep,yypvt[-0].nodep) ); } break;
case 78:
# line 364 "mip/cgram.y"
{  defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), uclass(curclass) );
			} break;
case 79:
# line 368 "mip/cgram.y"
{  doinit( yypvt[-0].nodep );
			    endinit(); } break;
case 80:
# line 371 "mip/cgram.y"
{  endinit(); } break;
case 83:
# line 380 "mip/cgram.y"
{  doinit( yypvt[-0].nodep ); } break;
case 84:
# line 382 "mip/cgram.y"
{  irbrace(); } break;
case 89:
# line 394 "mip/cgram.y"
{  werror( "old-fashioned initialization: use =" ); } break;
case 91:
# line 399 "mip/cgram.y"
{  ilbrace(); } break;
case 92:
# line 405 "mip/cgram.y"
{  --blevel;
			    if( blevel == 1 ) blevel = 0;
			    clearst( blevel );
			    checkst( blevel );
			    autooff = *--psavbc;
			    regvar = *--psavbc;
			    } break;
case 93:
# line 415 "mip/cgram.y"
{  if( blevel == 1 ) dclargs();
			    ++blevel;
			    if( psavbc > &asavbc[BCSZ-2] ) cerror( "nesting too deep" );
			    *psavbc++ = regvar;
			    *psavbc++ = autooff;
			    } break;
case 94:
# line 424 "mip/cgram.y"
{ ecomp( yypvt[-1].nodep ); } break;
case 96:
# line 427 "mip/cgram.y"
{ deflab(yypvt[-1].intval);
			   reached = 1;
			   } break;
case 97:
# line 431 "mip/cgram.y"
{  if( yypvt[-1].intval != NOLAB ){
				deflab( yypvt[-1].intval );
				reached = 1;
				}
			    } break;
case 98:
# line 437 "mip/cgram.y"
{  branch(  contlab );
			    deflab( brklab );
			    if( (flostat&FBRK) || !(flostat&FLOOP)) reached = 1;
			    else reached = 0;
			    resetbc(0);
			    } break;
case 99:
# line 444 "mip/cgram.y"
{  deflab( contlab );
			    if( flostat & FCONT ) reached = 1;
			    ecomp( buildtree( CBRANCH, buildtree( NOT, yypvt[-2].nodep, NIL ), bcon( yypvt[-6].intval ) ) );
			    deflab( brklab );
			    reached = 1;
			    resetbc(0);
			    } break;
case 100:
# line 452 "mip/cgram.y"
{  deflab( contlab );
			    if( flostat&FCONT ) reached = 1;
			    if( yypvt[-2].nodep ) ecomp( yypvt[-2].nodep );
			    branch( yypvt[-3].intval );
			    deflab( brklab );
			    if( (flostat&FBRK) || !(flostat&FLOOP) ) reached = 1;
			    else reached = 0;
			    resetbc(0);
			    } break;
case 101:
# line 462 "mip/cgram.y"
{  if( reached ) branch( brklab );
			    deflab( yypvt[-1].intval );
			   swend();
			    deflab(brklab);
			    if( (flostat&FBRK) || !(flostat&FDEF) ) reached = 1;
			    resetbc(FCONT);
			    } break;
case 102:
# line 470 "mip/cgram.y"
{  if( brklab == NOLAB ) uerror( "illegal break");
			    else if(reached) branch( brklab );
			    flostat |= FBRK;
			    if( brkflag ) goto rch;
			    reached = 0;
			    } break;
case 103:
# line 477 "mip/cgram.y"
{  if( contlab == NOLAB ) uerror( "illegal continue");
			    else branch( contlab );
			    flostat |= FCONT;
			    goto rch;
			    } break;
case 104:
# line 483 "mip/cgram.y"
{  retstat |= NRETVAL;
			    branch( retlab );
			rch:
			    if( !reached ) werror( "statement not reached");
			    reached = 0;
			    } break;
case 105:
# line 490 "mip/cgram.y"
{  register NODE *temp;
			    idname = curftn;
			    temp = buildtree( NAME, NIL, NIL );
			    temp->type = DECREF( temp->type );
			    temp = buildtree( RETURN, temp, yypvt[-1].nodep );
			    /* now, we have the type of the RHS correct */
			    temp->left->op = FREE;
			    temp->op = FREE;
			    ecomp( buildtree( FORCE, temp->right, NIL ) );
			    retstat |= RETVAL;
			    branch( retlab );
			    reached = 0;
			    } break;
case 106:
# line 504 "mip/cgram.y"
{  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, INT );
			    q->rval = idname = yypvt[-1].intval;
			    defid( q, ULABEL );
			    stab[idname].suse = -lineno;
			    branch( (int) stab[idname].offset );
			    goto rch;
			    } break;
case 111:
# line 518 "mip/cgram.y"
{  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, LABEL );
			    q->rval = yypvt[-1].intval;
			    defid( q, LABEL );
			    reached = 1;
			    } break;
case 112:
# line 525 "mip/cgram.y"
{  addcase(yypvt[-1].nodep);
			    reached = 1;
			    } break;
case 113:
# line 529 "mip/cgram.y"
{  reached = 1;
			    adddef();
			    flostat |= FDEF;
			    } break;
case 114:
# line 535 "mip/cgram.y"
{  savebc();
			    if( !reached ) werror( "loop not entered at top");
			    brklab = getlab();
			    contlab = getlab();
			    deflab( yyval.intval = getlab() );
			    reached = 1;
			    } break;
case 115:
# line 544 "mip/cgram.y"
{  ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( yyval.intval=getlab()) ) ) ;
			    reached = 1;
			    } break;
case 116:
# line 549 "mip/cgram.y"
{  if( reached ) branch( yyval.intval = getlab() );
			    else yyval.intval = NOLAB;
			    deflab( yypvt[-2].intval );
			    reached = 1;
			    } break;
case 117:
# line 557 "mip/cgram.y"
{  savebc();
			    if( !reached ) werror( "loop not entered at top");
			    if( yypvt[-1].nodep->op == ICON && yypvt[-1].nodep->lval != 0 ) flostat = FLOOP;
			    deflab( contlab = getlab() );
			    reached = 1;
			    brklab = getlab();
			    if( flostat == FLOOP ) tfree( yypvt[-1].nodep );
			    else ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( brklab) ) );
			    } break;
case 118:
# line 568 "mip/cgram.y"
{  if( yypvt[-3].nodep ) ecomp( yypvt[-3].nodep );
			    else if( !reached ) werror( "loop not entered at top");
			    savebc();
			    contlab = getlab();
			    brklab = getlab();
			    deflab( yyval.intval = getlab() );
			    reached = 1;
			    if( yypvt[-1].nodep ) ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( brklab) ) );
			    else flostat |= FLOOP;
			    } break;
case 119:
# line 580 "mip/cgram.y"
{  register NODE *temp; /* cast e to int ERR*/
			    savebc();
			    brklab = getlab();
			    temp = buildtree( CAST,
					block( NAME, NIL, NIL, INT, 0, INT ),
					yypvt[-1].nodep );
			    temp->op = FREE;
			    temp->left->op = FREE;
			    ecomp( buildtree( FORCE, temp->right, NIL ) );
			    branch( yyval.intval = getlab() );
			    swstart();
			    reached = 0;
			    } break;
case 120:
# line 595 "mip/cgram.y"
{ yyval.intval=instruct; stwart=instruct=0; } break;
case 121:
# line 597 "mip/cgram.y"
{  yyval.intval = icons( yypvt[-0].nodep );  instruct=yypvt[-1].intval; } break;
case 123:
# line 601 "mip/cgram.y"
{ yyval.nodep=0; } break;
case 125:
# line 606 "mip/cgram.y"
{  goto bop; } break;
case 126:
# line 610 "mip/cgram.y"
{
			preconf:
			    if( yychar==RELOP||yychar==EQUOP||yychar==AND||yychar==OR||yychar==ER ){
			    precplaint:
				if( hflag ) werror( "precedence confusion possible: parenthesize!" );
				}
			bop:
			    yyval.nodep = buildtree( yypvt[-1].intval, yypvt[-2].nodep, yypvt[-0].nodep );
			    } break;
case 127:
# line 620 "mip/cgram.y"
{  yypvt[-1].intval = COMOP;
			    goto bop;
			    } break;
case 128:
# line 624 "mip/cgram.y"
{  goto bop; } break;
case 129:
# line 626 "mip/cgram.y"
{  if(yychar==SHIFTOP) goto precplaint; else goto bop; } break;
case 130:
# line 628 "mip/cgram.y"
{  if(yychar==SHIFTOP ) goto precplaint; else goto bop; } break;
case 131:
# line 630 "mip/cgram.y"
{  if(yychar==PLUS||yychar==MINUS) goto precplaint; else goto bop; } break;
case 132:
# line 632 "mip/cgram.y"
{  goto bop; } break;
case 133:
# line 634 "mip/cgram.y"
{  goto preconf; } break;
case 134:
# line 636 "mip/cgram.y"
{  if( yychar==RELOP||yychar==EQUOP ) goto preconf;  else goto bop; } break;
case 135:
# line 638 "mip/cgram.y"
{  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; } break;
case 136:
# line 640 "mip/cgram.y"
{  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; } break;
case 137:
# line 642 "mip/cgram.y"
{  goto bop; } break;
case 138:
# line 644 "mip/cgram.y"
{  goto bop; } break;
case 139:
# line 646 "mip/cgram.y"
{  abop:
				yyval.nodep = buildtree( ASG yypvt[-2].intval, yypvt[-3].nodep, yypvt[-0].nodep );
				} break;
case 140:
# line 650 "mip/cgram.y"
{  goto abop; } break;
case 141:
# line 652 "mip/cgram.y"
{  goto abop; } break;
case 142:
# line 654 "mip/cgram.y"
{  goto abop; } break;
case 143:
# line 656 "mip/cgram.y"
{  goto abop; } break;
case 144:
# line 658 "mip/cgram.y"
{  goto abop; } break;
case 145:
# line 660 "mip/cgram.y"
{  goto abop; } break;
case 146:
# line 662 "mip/cgram.y"
{  goto abop; } break;
case 147:
# line 664 "mip/cgram.y"
{  yyval.nodep=buildtree(QUEST, yypvt[-4].nodep, buildtree( COLON, yypvt[-2].nodep, yypvt[-0].nodep ) );
			    } break;
case 148:
# line 667 "mip/cgram.y"
{  werror( "old-fashioned assignment operator" );  goto bop; } break;
case 149:
# line 669 "mip/cgram.y"
{  goto bop; } break;
case 151:
# line 673 "mip/cgram.y"
{  yyval.nodep = buildtree( yypvt[-0].intval, yypvt[-1].nodep, bcon(1) ); } break;
case 152:
# line 675 "mip/cgram.y"
{ ubop:
			    yyval.nodep = buildtree( UNARY yypvt[-1].intval, yypvt[-0].nodep, NIL );
			    } break;
case 153:
# line 679 "mip/cgram.y"
{  if( ISFTN(yypvt[-0].nodep->type) || ISARY(yypvt[-0].nodep->type) ){
				werror( "& before array or function: ignored" );
				yyval.nodep = yypvt[-0].nodep;
				}
			    else goto ubop;
			    } break;
case 154:
# line 686 "mip/cgram.y"
{  goto ubop; } break;
case 155:
# line 688 "mip/cgram.y"
{
			    yyval.nodep = buildtree( yypvt[-1].intval, yypvt[-0].nodep, NIL );
			    } break;
case 156:
# line 692 "mip/cgram.y"
{  yyval.nodep = buildtree( yypvt[-1].intval==INCR ? ASG PLUS : ASG MINUS,
						yypvt[-0].nodep,
						bcon(1)  );
			    } break;
case 157:
# line 697 "mip/cgram.y"
{  yyval.nodep = doszof( yypvt[-0].nodep ); } break;
case 158:
# line 699 "mip/cgram.y"
{  yyval.nodep = buildtree( CAST, yypvt[-2].nodep, yypvt[-0].nodep );
			    yyval.nodep->left->op = FREE;
			    yyval.nodep->op = FREE;
			    yyval.nodep = yyval.nodep->right;
			    } break;
case 159:
# line 705 "mip/cgram.y"
{  yyval.nodep = doszof( yypvt[-1].nodep ); } break;
case 160:
# line 707 "mip/cgram.y"
{  yyval.nodep = buildtree( UNARY MUL, buildtree( PLUS, yypvt[-3].nodep, yypvt[-1].nodep ), NIL ); } break;
case 161:
# line 709 "mip/cgram.y"
{  yyval.nodep=buildtree(UNARY CALL,yypvt[-1].nodep,NIL); } break;
case 162:
# line 711 "mip/cgram.y"
{  yyval.nodep=buildtree(CALL,yypvt[-2].nodep,yypvt[-1].nodep); } break;
case 163:
# line 713 "mip/cgram.y"
{  if( yypvt[-1].intval == DOT ){
				yypvt[-2].nodep = buildtree( UNARY AND, yypvt[-2].nodep, NIL );
				}
			    idname = yypvt[-0].intval;
			    yyval.nodep = buildtree( STREF, yypvt[-2].nodep, buildtree( NAME, NIL, NIL ) );
			    } break;
case 164:
# line 720 "mip/cgram.y"
{  idname = yypvt[-0].intval;
			    /* recognize identifiers in initializations */
			    if( blevel==0 && stab[idname].stype == UNDEF ) {
				register NODE *q;
				werror( "undeclared initializer name %.8s", stab[idname].sname );
				q = block( FREE, NIL, NIL, INT, 0, INT );
				q->rval = idname;
				defid( q, EXTERN );
				}
			    yyval.nodep=buildtree(NAME,NIL,NIL);
			    stab[yypvt[-0].intval].suse = -lineno;
			} break;
case 165:
# line 733 "mip/cgram.y"
{  yyval.nodep=bcon(0);
			    yyval.nodep->lval = lastcon;
			    yyval.nodep->rval = NONAME;
			    if( yypvt[-0].intval ) yyval.nodep->csiz = yyval.nodep->type = ctype(LONG);
			    } break;
case 166:
# line 739 "mip/cgram.y"
{  yyval.nodep=buildtree(FCON,NIL,NIL);
			    yyval.nodep->dval = dcon;
			    } break;
case 167:
# line 743 "mip/cgram.y"
{  yyval.nodep = getstr(); /* get string contents */ } break;
case 168:
# line 745 "mip/cgram.y"
{ yyval.nodep=yypvt[-1].nodep; } break;
case 169:
# line 749 "mip/cgram.y"
{
			yyval.nodep = tymerge( yypvt[-1].nodep, yypvt[-0].nodep );
			yyval.nodep->op = NAME;
			yypvt[-1].nodep->op = FREE;
			} break;
case 170:
# line 757 "mip/cgram.y"
{ yyval.nodep = bdty( NAME, NIL, -1 ); } break;
case 171:
# line 759 "mip/cgram.y"
{ yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,-1),0); } break;
case 172:
# line 761 "mip/cgram.y"
{  yyval.nodep = bdty( UNARY CALL, yypvt[-3].nodep, 0 ); } break;
case 173:
# line 763 "mip/cgram.y"
{  goto umul; } break;
case 174:
# line 765 "mip/cgram.y"
{  goto uary; } break;
case 175:
# line 767 "mip/cgram.y"
{  goto bary;  } break;
case 176:
# line 769 "mip/cgram.y"
{ yyval.nodep = yypvt[-1].nodep; } break;
case 177:
# line 773 "mip/cgram.y"
{  if( stab[yypvt[-1].intval].stype == UNDEF ){
				register NODE *q;
				q = block( FREE, NIL, NIL, FTN|INT, 0, INT );
				q->rval = yypvt[-1].intval;
				defid( q, EXTERN );
				}
			    idname = yypvt[-1].intval;
			    yyval.nodep=buildtree(NAME,NIL,NIL);
			    stab[idname].suse = -lineno;
			} break;
		}
		goto yystack;  /* stack new state and value */

	}
