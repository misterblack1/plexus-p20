# define NAME 257
# define CONST 258
# define FLABEL 259
# define DREG 260
# define IF 261
# define GOTO 262
# define EQL 263
# define NEQ 264
# define EQ 265
# define XOR 266
# define INC 267
# define DEC 268
# define LSS 269
# define LEQ 270
# define GTR 271
# define GEQ 272
# define OR 273
# define AND 274
# define COMP 275
# define NOT 276
# define CO 277
# define SC 278
# define CM 279
# define RP 280
# define LP 281
# define LC 282
# define AM 283
# define RC 284
# define LB 285
# define RB 286
# define CONTINUE 287
# define FUNCTION 288
# define REPOP 289
# define GET 290
# define GETA 291
# define XMT 292
# define PUT 293
# define CRC16 294
# define LRCGEN 295
# define RETURN 296
# define RCV 297
# define TIMEOUT 298
# define RTNERR 299
# define SNAP 300
# define RTNXBUF 301
# define RTNRBUF 302
# define EXIT 303
# define END 304
# define PUTEA 305
# define GETXBUF 306
# define GETRBUF 307
# define CRCLOC 308
# define XSOM 309
# define XEOM 310
# define RSXBUF 311
# define RSRBUF 312
# define RSOM 313
# define SUB 314
# define ADD 315
# define FNAME 316
# define ARRAY 317
# define TESTOP 318
# define TRACE 319
# define LSH 320
# define RSH 321
# define RCVFRM 322
# define RSRFRM 323
# define RTNRFRM 324
# define XMTBUSY 325
# define GETXFRM 326
# define XMTFRM 327
# define RTNXFRM 328
# define SETCTL 329
# define RSXMTQ 330
# define XMTCTL 331
# define ABTXFRM 332
# define TIMER 333
# define GETCMD 334
# define RTNSTAT 335
# define ATOE 336
# define ETOA 337
# define DSRWAIT 338
# define NORBUF 339
# define RTNRPT 340

# line 13 "pl.y"
#define ZERO	1
#define OUNA	2
#define BINA	3
char fname[32],priex[32],bname[32],aname[32],offset[32];
char oline[80],ocode[256];
int stmtype,atype;
char *repop[] = {
	"MOVM",
	"ORM",
	"XORM",
	"ADDM",
	"SUBM",
	"ANDM",
	"LSHM",
	"RSHM"
	};
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 556 "pl.y"

int debug = 0;
int yyline = 1;
#include "pl.yl"
char	desreg[32],srcreg[32];
main(argc,argv)
int argc;
char *argv[];
{
	yyinit(argc,argv);
	if(yyparse())
		return;
	yyaccpt();
}
yyinit(argc,argv)
int argc;
char *argv[];
{
	while(argc > 1 && argv[1][0] == '-'){
		switch(argv[1][1]){
			case 'd':
				debug = atoi(&argv[1][2]);
				if(debug == 1)
					printf("debug = %d\n",debug);
				break;
		}
		argc--;
		argv++;
	}
}
yyaccpt()
{
}
yyerror(s)
char *s;
{
	extern int yyline;
	fprintf(stderr,"\n***** pl: %s",s);
	if(yyline)
		fprintf(stderr,",input line %d.\n",yyline);
}
bopcode(s,lside,rside)
char *s;
int lside,rside;
{
	switch(lside){
		case CONST: case ZERO:
			sprintf(oline,"	MOVI	%s\n",priex);
			strcat(ocode,oline);
			break;
		case NAME:
			sprintf(oline,"	MOV	%s\n",priex);
			strcat(ocode,oline);
	}
	if(rside == NAME){
		sprintf(oline,"	%s	%s\n",s,name);
		strcat(ocode,oline);
	}else{
		sprintf(oline,"	%sI	%s\n",s,name);
		strcat(ocode,oline);
	}
}
sccode(argct,arg1,arg2,priname)
char *priname;
{
	switch(argct){
	case 0:
		sprintf(oline,"	%s\n",priname);
		strcat(ocode,oline);
		break;
	case 1:
		if(arg1 == NAME){
			sprintf(oline,"	%s	%s\n",priname,name);
			strcat(ocode,oline);
		}else{
			sprintf(oline,"	%sI	%s\n",priname,name);
			strcat(ocode,oline);
		}
		break;
	case 2:
		if(arg2 == NAME){
			sprintf(oline,"	MOV	%s\n",name);
			strcat(ocode,oline);
		}else{
			sprintf(oline,"	MOVI	%s\n",name);
			strcat(ocode,oline);
		}
		if(arg1 == atype){
			sprintf(oline,"	%s	%s\n",priname,aname);
			strcat(ocode,oline);
		}else{
			sprintf(oline,"	%sI	%s\n",priname,aname);
			strcat(ocode,oline);
		}
	}
}
prline()
{
	if(debug == 2){
		printf("*%d	%s\n",yyline,sline);
		sline[0]='\0';
	}
	printf("%s",ocode);
	ocode[0]='\0';
}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 30,
	285, 100,
	-2, 97,
-1, 111,
	285, 100,
	-2, 97,
-1, 115,
	285, 100,
	-2, 97,
	};
# define YYNPROD 101
# define YYLAST 593
short yyact[]={

  27,  30,  40,  31, 276,  20,  21,  92,  86,   9,
 270,  33,  34, 225, 182,  83,  82, 108, 158,  38,
  37,  82,  25, 157, 156,  39, 155, 154, 153, 152,
 151,  22, 150, 149,  41, 148,  42,  47,  48, 147,
  23,  43,  54,  55,  57,  58,  59,  64,  16, 146,
  60,  61,  52,  44,  46,  63,  62,  45, 145, 144,
  24, 143,  49,  56, 142, 141,  65,  71,  72,  73,
  67,  68,  70,  66,  75,  69,  76,  79,  77,  80,
  50,  51,  53,  74,  78,  27,  30,  40,  31, 140,
  20,  21, 139, 111,  40, 138,  33,  34, 137, 136,
 111,  40, 135, 134,  38,  37, 133,  25, 132, 131,
  39, 173, 130, 129, 128, 127,  22, 168, 126,  41,
 125,  42,  47,  48, 168,  23,  43,  54,  55,  57,
  58,  59,  64,   7, 124,  60,  61,  52,  44,  46,
  63,  62,  45,   9, 123,  24, 122,  49,  56, 121,
 120,  65,  71,  72,  73,  67,  68,  70,  66,  75,
  69,  76,  79,  77,  80,  50,  51,  53,  74,  78,
 115,  40,   6, 119,  87, 284, 283, 184, 268, 264,
  33,  34, 263, 262, 261, 260, 259, 258,  38,  37,
 256, 255, 254, 272,  39, 253, 252, 251, 250, 248,
 247, 246, 245,  41, 244,  42,  47,  48, 243, 242,
  43,  54,  55,  57,  58,  59,  64, 241, 240,  60,
  61,  52,  44,  46,  63,  62,  45, 239, 238, 237,
 236,  49,  56, 235, 234,  65,  71,  72,  73,  67,
  68,  70,  66,  75,  69,  76,  79,  77,  80,  50,
  51,  53,  74,  78, 115,  40, 227, 220, 219, 218,
 217, 216, 215, 213,  33,  34, 207, 206, 197, 183,
 159,  32,  38, 163, 271, 275, 285, 282,  39, 280,
 279, 269, 266, 265, 165, 164,  94,  41,  91,  42,
  47,  48,  90,  85,  43,  54,  55,  57,  58,  59,
  64,  84,  81,  60,  61,  52,  44,  46,  63,  62,
  45, 118, 267, 274, 273,  49,  56,  88,  89,  65,
  71,  72,  73,  67,  68,  70,  66,  75,  69,  76,
  79,  77,  80,  50,  51,  53,  74,  78, 102, 103,
 281,  98, 233, 223, 104, 105, 106, 107,  97,  99,
 190, 189, 188, 160, 224, 222, 221, 111, 210, 209,
 205, 204, 203, 202, 201, 196,  14, 167, 169, 170,
 171, 172, 174, 175, 176, 177, 178, 179, 180, 181,
  11, 249,  28,  18,  17,  15, 109,   5,  36,  96,
  95,  10, 186,  35,  29, 100, 101, 191, 192, 193,
 194, 195, 114,  13, 198, 199, 200,  26,  12,   8,
   4,  93,   3,   2, 208,  19,   1,   0, 212,   0,
 113, 116,   0, 117,   0,   0,   0,   0,   0,  19,
   0,   0,   0,   0,   0,   0, 110, 112,   0,   0,
 231,   0,   0,   0,   0, 232,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 162, 161,   0,   0,   0,   0,   0, 166,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 185,   0, 187,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 277, 278, 226, 229, 228, 211,   0,
 230, 214,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0, 257 };
short yypact[]={

-1000,-1000,-145,-279,-1000,-1000, 123,-1000,-1000, 109,
-1000,-1000,-256,  24,-265,-270,  23,-1000,  15,-281,
-107,  60,  14,  10,-282,-1000,-171,   8,  75,-1000,
-260,-1000,-1000, 100, 100,-1000,-1000, -87, -87, -87,
-1000,-108,-131,-132,-135,-137,-147,-161,-163,-166,
-167,-168,-169,-172,-173,-175,-178,-179,-182,-183,
-186,-189,-192,-216,-217,-220,-222,-223,-232,-242,
-246,-248,-249,-251,-252,-253,-254,-255,-257,-258,
-263,-1000, -10,  95,-1000,-1000,-1000,  -3,   7,   6,
-1000,-1000,-1000,-1000,-1000,-157,-157,-157,-157,-164,
-157,-157,-157,-157,-157,-157,-157,-157,-1000,-271,
-1000,-1000,-1000,-1000,-1000,-265,-1000, -11,-103, 100,
-157, 100,  94,  93,  92,-157,-157,-157,-157,-157,
 108, -12,-157,-157,-157, 107, 106, 105, 104, 103,
 -13, -14,-157, 102, 101, 100,-157, -17, 100, -18,
 -19, -20, -21, -22, -23,  99,  98,  85,  97,-1000,
-273, -87, -24, -87,-1000,-1000, -87,-1000,-157,-1000,
-1000,-1000,-1000,-157,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,  84,-1000,-1000, -46, -47, -50, -51, -52,
 -53, -62, -63, -71, -72, -76, -78,-1000, -79, -80,
 -81, -82, -83, -84, -85, -88,-1000,-1000, -89, -90,
-1000, -93, -94,-1000, -95,-1000,-1000,-1000,-1000,-1000,
-1000, -96, -97, -98,-101,   5,   4,  50,-102,  75,
   3,-103,-1000,-276,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,  -5,
-1000,-1000,-1000,-1000,-1000,-1000,-1000, -86,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,  56,  13,-292,
-1000,-157,-157,   2,   1,  82,  -1,-104,-105,-1000,
-1000,  -2,-1000,-1000,-1000,-1000 };
short yypgo[]={

   0, 416, 413, 412, 410, 385, 387, 409, 408, 393,
 384, 383, 402, 407, 382, 394, 271, 388, 381, 386 };
short yyr1[]={

   0,   1,   2,   2,   4,   4,   3,   3,   6,   7,
   9,   8,   8,  10,  10,  10,  10,  10,  10,  10,
  10,  10,  10,  10,  10,  10,  11,  11,  13,  13,
  14,  14,  14,  14,  14,  14,  14,  14,  15,  15,
  15,  15,  15,  15,  15,  15,  15,  15,  15,  15,
  15,  15,  17,  17,  17,  17,  17,  17,  17,  17,
  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,
  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,
  17,  17,  17,  17,  17,  17,  17,  17,  17,  17,
  17,  17,  17,  16,  16,  16,  12,  12,   5,  18,
  19 };
short yyr2[]={

   0,   2,   0,   2,   7,   1,   1,   2,   4,   3,
   3,   0,   2,   2,   5,   7,   8,   7,   3,   3,
   2,   2,   7,   1,   2,   2,   1,   1,   2,   1,
   1,   2,   2,   1,   1,   2,   2,   3,   3,   3,
   3,   3,   3,   3,   3,   4,   3,   3,   3,   3,
   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,
   4,   4,   4,   4,   3,   4,   4,   4,   7,   4,
   4,   4,   4,   4,   3,   3,   4,   4,   7,   4,
   4,   3,   4,   3,   3,   3,   3,   3,   3,   4,
   4,   4,   4,   1,   1,   3,   5,   1,   0,   0,
   0 };
short yychk[]={

-1000,  -1,  -2,  -3,  -4,  -6, 317, 278,  -7, 288,
  -6, 257,  -8,  -9, 257,  -5, 304, -10, -11, -12,
 261, 262, 287, 296, 316, 278, -13, 256, -14, -15,
 257, 259, -16, 267, 268,  -9, -17, 276, 275, 281,
 258, 290, 292, 297, 309, 313, 310, 293, 294, 318,
 336, 337, 308, 338, 298, 299, 319, 300, 301, 302,
 306, 307, 312, 311, 303, 322, 329, 326, 327, 331,
 328, 323, 324, 325, 339, 330, 332, 334, 340, 333,
 335, 278, 281, 285, 278, 278, 289, 281, 257, 258,
 278, 278, 289, -10, 278, 315, 314, 273, 266, 274,
 320, 321, 263, 264, 269, 270, 271, 272, 277, -19,
 -12, 257, -12, -14, -12, 257, -14, -11, -16, 281,
 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
 281, 281, 281, 281, 281, 281, 281, 281, 281, 280,
 258,  -5, -11, 276, 278, 278,  -5, -16, 281, -16,
 -16, -16, -16, 275, -16, -16, -16, -16, -16, -16,
 -16, -16, 285, 280, 280, -12, -16, -12, 258, 258,
 258, -16, -16, -16, -16, -16, 257, 280, -16, -16,
 -16, 257, 257, 257, 257, 257, 280, 280, -16, 257,
 257, -12, -16, 280, -12, 280, 280, 280, 280, 280,
 280, 257, 257, 258, 257, 286, -11, 280, -11, -14,
 -11, -16, -16, 258, 280, 280, 280, 280, 280, 280,
 280, 280, 280, 280, 280, 280, 280, 280, 280, -18,
 280, 280, 280, 280, 280, 280, 280, -18, 280, 280,
 280, 280, 280, 280, 280, 278, 278, 262, 280, 278,
 286, 279, 279, 258, 257, 262, 296, -16, -16, 278,
 278, 258, 278, 280, 280, 278 };
short yydef[]={

   2,  -2,   0,   1,   3,   6,   0,   5,  11,   0,
   7,  98,   0,   0,   0,   0,   0,  12,   0,  93,
   0,   0,   0,   0,   0,  23,   0,   0,  26,  27,
  -2,  29,  30,   0,   0,  33,  34,   0,   0,   0,
  94,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   9,   0,   0,   8,  13,  98,   0,   0,   0,
  20,  21,  98,  24,  25,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  28,   0,
  31,  -2,  32,  35,  93,  -2,  36,   0,  30,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  10,
   0,   0,   0,   0,  18,  19,   0,  38,   0,  39,
  40,  41,  42,   0,  43,  44,  46,  47,  48,  49,
  50,  51,   0,  37,  95,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  64,   0,   0,
  99,   0,   0,   0,   0,   0,  74,  75,   0,   0,
  99,   0,   0,  81,   0,  83,  84,  85,  86,  87,
  88,   0,   0,   0,   0,   0,   0,   0,   0,  26,
   0,   0,  45,   0,  52,  53,  54,  55,  56,  57,
  58,  59,  60,  61,  62,  63,  65,  66,  67,   0,
  69,  70,  71,  72,  73,  76,  77,   0,  79,  80,
  82,  89,  90,  91,  92,   4,  14,   0,   0,   0,
  96,   0,   0,   0,   0,   0,   0,   0,   0,  15,
  17,   0,  22,  68,  78,  16 };
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
			
case 4:
# line 34 "pl.y"

		{
			sprintf(oline,"%s	BSS	%s\n",desreg,name);
			strcat(ocode,oline);
			prline();
		} break;
case 5:
# line 40 "pl.y"

		{
			if(yypvt[-0] == 1)prline();
		} break;
case 8:
# line 46 "pl.y"

		{
			if(stmtype != 7){
				sprintf(oline,"	RETURNI	0\n");
				strcat(ocode,oline);
				prline();
			}
			stmtype = 0;
		} break;
case 9:
# line 55 "pl.y"

		{
			sprintf(oline,"%s:\n",name);
			strcat(ocode,oline);
			fname[0] = '\0';
			strcpy(fname,name);
			prline();
		} break;
case 13:
# line 67 "pl.y"

		{
			switch(yypvt[-1]){
			case CONST: case ZERO:
				sprintf(oline,"	MOVI	%s\n",name);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",name);
				strcat(ocode,oline);
				break;
			}
			stmtype = 1;
			prline();
		} break;
case 14:
# line 82 "pl.y"

		{
			if(debug == 1)
				printf("a  %d %d %d %d\n",yypvt[-4],yypvt[-3],yypvt[-2],yypvt[-1]);
			stmtype = 2;
			switch(yypvt[-1]){
			case ZERO:
				sprintf(oline,"	CLR	%s\n",desreg);
				strcat(ocode,oline);
				break;
			case CONST:
				sprintf(oline,"	MOVI	%s\n",name);
				strcat(ocode,oline);
				sprintf(oline,"	%s	%s\n",repop[yypvt[-3]],desreg);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",name);
				strcat(ocode,oline);
				sprintf(oline,"	%s	%s\n",repop[yypvt[-3]],desreg);
				strcat(ocode,oline);
				break;
			default:
				sprintf(oline,"	%s	%s\n",repop[yypvt[-3]],desreg);
				strcat(ocode,oline);
			}
			prline();
		} break;
case 15:
# line 110 "pl.y"
 
		{
			stmtype = 3;
			switch(yypvt[-4]){
			case CONST: case ZERO:
				sprintf(oline,"	MOVI	%s\n",priex);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",priex);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	JMPNEZ	L%s\n",name);
			strcat(ocode,oline);
			prline();
		} break;
case 16:
# line 127 "pl.y"
 
		{
			stmtype = 3;
			switch(yypvt[-4]){
			case CONST: case ZERO:
				sprintf(oline,"	MOVI	%s\n",priex);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",priex);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	JMPEQZ	L%s\n",name);
			strcat(ocode,oline);
			prline();
		} break;
case 17:
# line 144 "pl.y"
 
		{
			stmtype = 3;
			switch(yypvt[-4]){
			case CONST: case ZERO:
				sprintf(oline,"	MOVI	%s\n",priex);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",priex);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	JMPNEZ	%s\n",name);
			strcat(ocode,oline);
			prline();
		} break;
case 18:
# line 161 "pl.y"
 
		{
			stmtype = 4;
			sprintf(oline,"	JMP	%s\n",name);
			strcat(ocode,oline);
			prline();
		} break;
case 19:
# line 168 "pl.y"
 
		{
			stmtype = 4;
			sprintf(oline,"	JMP	L%s\n",name);
			strcat(ocode,oline);
			prline();
		} break;
case 20:
# line 175 "pl.y"

		{
			stmtype = 5;
			prline();
		} break;
case 21:
# line 180 "pl.y"

		{
			stmtype = 7;
			sprintf(oline,"	RETURNI	0\n");
			strcat(ocode,oline);
			prline();
		} break;
case 22:
# line 187 "pl.y"

		{
			switch(yypvt[-3]){
			case CONST:
				sprintf(oline,"	RETURNI	%s\n",name);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	RETURN	%s\n",name);
				strcat(ocode,oline);
				break;
			default:
				sprintf(oline,"	%s	_%s\n",repop[yypvt[-5]],desreg);
				strcat(ocode,oline);
				sprintf(oline,"	RETURN	_%s\n",desreg);
				strcat(ocode,oline);
			}
			stmtype = 7;
			prline();
		} break;
case 23:
# line 207 "pl.y"

		{
			stmtype = 6;
			if(yypvt[-0] == 1)prline();
		} break;
case 24:
# line 212 "pl.y"

		{
		} break;
case 27:
# line 217 "pl.y"

		{
			yyval = BINA;
			if(debug == 1)
				printf("d  %d\n",yypvt[-0]);
		} break;
case 28:
# line 223 "pl.y"

		{
			sprintf(oline,"%s:\n",name);
			strcat(ocode,oline);
		} break;
case 29:
# line 228 "pl.y"

		{
			sprintf(oline,"L%s:\n",name);
			strcat(ocode,oline);
		} break;
case 30:
# line 233 "pl.y"

		{
			if(debug == 1)
				printf("b  %d\n",yypvt[-0]);
			yyval = yypvt[-0];
			strcpy(priex,name);
		} break;
case 31:
# line 240 "pl.y"
 
		{
			sprintf(oline,"	INC	%s\n",name);
			strcat(ocode,oline);
			yyval = OUNA;
		} break;
case 32:
# line 246 "pl.y"
 
		{
			sprintf(oline,"	DEC	%s\n",name);
			strcat(ocode,oline);
			yyval = OUNA;
		} break;
case 33:
# line 252 "pl.y"

		{
			sprintf(oline,"	CALL	%s\n",name);
			strcat(ocode,oline);
			yyval = OUNA;
		} break;
case 34:
# line 258 "pl.y"

		{
			yyval = OUNA;
		} break;
case 35:
# line 262 "pl.y"

		{
			switch(yypvt[-0]){
			case ZERO: case CONST:
				sprintf(oline,"	MOVI	%s\n",name);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",name);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	TSTEQLI	0\n");
			strcat(ocode,oline);
			yyval = OUNA;
		} break;
case 36:
# line 278 "pl.y"

		{
			switch(yypvt[-0]){
			case ZERO: case CONST:
				sprintf(oline,"	MOVI	%s\n",name);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",name);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	XORI	0377\n");
			strcat(ocode,oline);
			yyval = OUNA;
		} break;
case 37:
# line 294 "pl.y"

		{
			yyval = yypvt[-1];
		} break;
case 38:
# line 298 "pl.y"

		{
			bopcode("ADD",yypvt[-2],yypvt[-0]);
		} break;
case 39:
# line 302 "pl.y"

		{
			bopcode("SUB",yypvt[-2],yypvt[-0]);
		} break;
case 40:
# line 306 "pl.y"

		{
			bopcode("OR",yypvt[-2],yypvt[-0]);
		} break;
case 41:
# line 310 "pl.y"

		{
			bopcode("XOR",yypvt[-2],yypvt[-0]);
		} break;
case 42:
# line 314 "pl.y"

		{
			bopcode("AND",yypvt[-2],yypvt[-0]);
		} break;
case 43:
# line 318 "pl.y"

		{
			bopcode("LSH",yypvt[-2],yypvt[-0]);
		} break;
case 44:
# line 322 "pl.y"

		{
			bopcode("RSH",yypvt[-2],yypvt[-0]);
		} break;
case 45:
# line 326 "pl.y"

		{
			bopcode("BIC",yypvt[-3],yypvt[-0]);
		} break;
case 46:
# line 330 "pl.y"

		{
			bopcode("TSTEQL",yypvt[-2],yypvt[-0]);
		} break;
case 47:
# line 334 "pl.y"

		{
			bopcode("TSTNEQ",yypvt[-2],yypvt[-0]);
		} break;
case 48:
# line 338 "pl.y"

		{
			bopcode("TSTLSS",yypvt[-2],yypvt[-0]);
		} break;
case 49:
# line 342 "pl.y"

		{
			bopcode("TSTLEQ",yypvt[-2],yypvt[-0]);
		} break;
case 50:
# line 346 "pl.y"

		{
			bopcode("TSTGTR",yypvt[-2],yypvt[-0]);
		} break;
case 51:
# line 350 "pl.y"

		{
			bopcode("TSTGEQ",yypvt[-2],yypvt[-0]);
		} break;
case 52:
# line 354 "pl.y"

		{
			sccode(1,yypvt[-1],0,"GETBYTE");
		} break;
case 53:
# line 358 "pl.y"

		{
			sccode(1,yypvt[-1],0,"XMT");
		} break;
case 54:
# line 362 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RCV");
		} break;
case 55:
# line 366 "pl.y"

		{
			sccode(1,yypvt[-1],0,"XSOM");
		} break;
case 56:
# line 370 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RSOM");
		} break;
case 57:
# line 374 "pl.y"

		{
			sccode(1,yypvt[-1],0,"XEOM");
		} break;
case 58:
# line 378 "pl.y"

		{
			sccode(1,yypvt[-1],0,"PUTBYTE");
		} break;
case 59:
# line 382 "pl.y"

		{
			sccode(1,yypvt[-1],0,"CRC16");
		} break;
case 60:
# line 386 "pl.y"

		{
			sccode(1,yypvt[-1],0,"TESTOP");
		} break;
case 61:
# line 390 "pl.y"

		{
			sccode(1,yypvt[-1],0,"ATOE");
		} break;
case 62:
# line 394 "pl.y"

		{
			sccode(1,yypvt[-1],0,"ETOA");
		} break;
case 63:
# line 398 "pl.y"

		{
			sccode(1,yypvt[-1],0,"CRCLOC");
		} break;
case 64:
# line 402 "pl.y"

		{
			sccode(0,0,0,"DSRWAIT");
		} break;
case 65:
# line 406 "pl.y"

		{
			sccode(1,yypvt[-1],0,"TIMEOUT");
		} break;
case 66:
# line 410 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RTNERR");
		} break;
case 67:
# line 414 "pl.y"

		{
			sccode(1,yypvt[-1],0,"TRACE1");
		} break;
case 68:
# line 418 "pl.y"

		{
			sccode(2,yypvt[-4],yypvt[-1],"TRACE2");
		} break;
case 69:
# line 422 "pl.y"

		{
			sccode(1,yypvt[-1],0,"SNAP");
		} break;
case 70:
# line 426 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RTNXBUF");
		} break;
case 71:
# line 430 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RTNRBUF");
		} break;
case 72:
# line 434 "pl.y"

		{
			sccode(1,yypvt[-1],0,"GETXBUF");
		} break;
case 73:
# line 438 "pl.y"

		{
			sccode(1,yypvt[-1],0,"GETRBUF");
		} break;
case 74:
# line 442 "pl.y"

		{
			sccode(0,0,0,"RSRBUF");
		} break;
case 75:
# line 446 "pl.y"

		{
			sccode(0,0,0,"RSXBUF");
		} break;
case 76:
# line 450 "pl.y"

		{
			sccode(1,yypvt[-1],0,"HALT");
		} break;
case 77:
# line 454 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RCVFRM");
		} break;
case 78:
# line 458 "pl.y"

		{
			sccode(2,yypvt[-4],yypvt[-1],"SETCTL");
		} break;
case 79:
# line 462 "pl.y"

		{
			sccode(1,yypvt[-1],0,"GETXFRM");
		} break;
case 80:
# line 466 "pl.y"

		{
			sccode(1,yypvt[-1],0,"XMTFRM");
		} break;
case 81:
# line 470 "pl.y"

		{
			sccode(0,yypvt[-0],0,"XMTCTL");
		} break;
case 82:
# line 474 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RTNXFRM");
		} break;
case 83:
# line 478 "pl.y"

		{
			sccode(0,0,0,"RSRFRM");
		} break;
case 84:
# line 482 "pl.y"

		{
			sccode(0,0,0,"RTNRFRM");
		} break;
case 85:
# line 486 "pl.y"

		{
			sccode(0,0,0,"XMTBUSY");
		} break;
case 86:
# line 490 "pl.y"

		{
			sccode(0,0,0,"NORBUF");
		} break;
case 87:
# line 494 "pl.y"

		{
			sccode(0,0,0,"RSXMTQ");
		} break;
case 88:
# line 498 "pl.y"

		{
			sccode(0,0,0,"ABTXFRM");
		} break;
case 89:
# line 502 "pl.y"

		{
			sccode(1,yypvt[-1],0,"GETCMD");
		} break;
case 90:
# line 506 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RTNRPT");
		} break;
case 91:
# line 510 "pl.y"

		{
			sccode(1,yypvt[-1],0,"TIMER");
		} break;
case 92:
# line 514 "pl.y"

		{
			sccode(1,yypvt[-1],0,"RTNSTAT");
		} break;
case 94:
# line 519 "pl.y"

		{
			if(strcmp(name,"0") == 0)
				yyval = ZERO;
			else
				yyval = CONST;
		} break;
case 95:
# line 526 "pl.y"

		{
			yyval = yypvt[-1];
		} break;
case 96:
# line 530 "pl.y"

		{
			strcpy(&offset[0],name);
			strcpy(&name[0],bname);
			strcat(name,"+");
			strcat(name,offset);
			yyval = NAME;
		} break;
case 97:
# line 538 "pl.y"

		{
			yyval = NAME;
			if(debug == 1)
					printf("e  %d\n",yypvt[-0]);
		} break;
case 98:
# line 544 "pl.y"
{
			strcpy(desreg,name);
		} break;
case 99:
# line 547 "pl.y"
{
			strcpy(&aname[0],name);
			atype = NAME;
		} break;
case 100:
# line 552 "pl.y"
{
			strcpy(&bname[0],name);
			atype = NAME;
		} break;
		}
		goto yystack;  /* stack new state and value */

	}
