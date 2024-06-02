# define number 257
# define stop 258
# define DEFINED 259
# define EQ 260
# define NE 261
# define LE 262
# define GE 263
# define LS 264
# define RS 265
# define ANDAND 266
# define OROR 267
# define UMINUS 268
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

# line 79 "cpy.y"

# include "yylex.c"
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 44,
	262, 0,
	263, 0,
	60, 0,
	62, 0,
	-2, 9,
-1, 45,
	262, 0,
	263, 0,
	60, 0,
	62, 0,
	-2, 10,
-1, 46,
	262, 0,
	263, 0,
	60, 0,
	62, 0,
	-2, 11,
-1, 47,
	262, 0,
	263, 0,
	60, 0,
	62, 0,
	-2, 12,
-1, 48,
	260, 0,
	261, 0,
	-2, 13,
-1, 49,
	260, 0,
	261, 0,
	-2, 14,
	};
# define YYNPROD 30
# define YYLAST 363
short yyact[]={

  13,  24,  35,  58,  13,  11,  14,  30,  15,  11,
  12,  60,  13,  24,  12,   1,  57,  11,  14,  30,
  15,  59,  12,  18,  13,  19,  29,   0,   0,  11,
  14,   0,  15,   0,  12,  18,   3,  19,  29,  13,
  24,  31,  32,  33,  11,  14,  30,  15,   0,  12,
  13,  24,   0,   0,   0,  11,  14,  25,  15,   5,
  12,   0,  18,   0,  19,  29,   7,   0,   0,  25,
   0,   4,   0,  18,   0,  19,  29,   0,  13,  24,
   0,   0,   0,  11,  14,   0,  15,  26,  12,   0,
   0,   0,  13,  24,   0,   0,  25,  11,  14,  26,
  15,  18,  12,  19,  13,  24,   0,  25,   0,  11,
  14,   0,  15,  13,  12,  18,   0,  19,  11,  14,
   0,  15,   0,  12,   0,   0,  26,  18,   0,  19,
   0,   0,   0,   0,  13,  25,  18,  26,  19,  11,
  14,   0,  15,   0,  12,  13,   0,   0,   0,  25,
  11,  14,   6,  15,   0,  12,   0,  18,   0,  19,
   0,   0,   0,   0,   0,  26,   0,   0,   0,   0,
   0,   0,   2,   0,   0,   0,   0,   0,   0,  26,
  34,   0,   0,   0,  37,  38,  39,  40,  41,  42,
  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,
  53,  54,  55,  56,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,
   0,   0,   0,  22,  23,  20,  21,  16,  17,  27,
  28,   0,  61,   0,   0,  22,  23,  20,  21,  16,
  17,  27,  28,   0,   0,   0,   0,   0,   0,   0,
   0,  16,  17,   0,   0,   0,   0,   0,   0,   0,
  10,   0,  22,  23,  20,  21,  16,  17,  27,  28,
   0,   0,   0,  22,  23,  20,  21,  16,  17,  27,
  28,   0,   0,   9,   0,   8,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  22,  23,  20,  21,  16,  17,  27,   0,   0,
   0,   0,   0,   0,   0,  22,  23,  20,  21,  16,
  17,   0,   0,   0,   0,   0,   0,  22,  23,  20,
  21,  16,  17,   0,   0,   0,  22,  23,  20,  21,
  16,  17,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  20,
  21,  16,  17 };
short yypact[]={

  26,-1000,   2,-1000,  26,  26,  26,  26, -38,-1000,
-1000,  26,  26,  26,  26,  26,  26,  26,  26,  26,
  26,  26,  26,  26,  26,  26,  26,  26,  26,  26,
  26,-1000,-1000,-1000, -25,-254,-1000,-1000,-1000,-1000,
 -33, -33, 108, 108, -13, -13, -13, -13,  97,  97,
  76,  67,  67,  55,  41, -37,  13,-1000, -30,  26,
-1000,  13 };
short yypgo[]={

   0,  15, 172,  36 };
short yyr1[]={

   0,   1,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   3,   3,   3,   3,   3,   3,   3 };
short yyr2[]={

   0,   2,   3,   3,   3,   3,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   5,   3,   1,   2,   2,   2,   3,   4,   2,   1 };
short yychk[]={

-1000,  -1,  -2,  -3,  45,  33, 126,  40, 259, 257,
 258,  42,  47,  37,  43,  45, 264, 265,  60,  62,
 262, 263, 260, 261,  38,  94, 124, 266, 267,  63,
  44,  -3,  -3,  -3,  -2,  40, 257,  -2,  -2,  -2,
  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,
  -2,  -2,  -2,  -2,  -2,  -2,  -2,  41, 257,  58,
  41,  -2 };
short yydef[]={

   0,  -2,   0,  22,   0,   0,   0,   0,   0,  29,
   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  23,  24,  25,   0,   0,  28,   2,   3,   4,
   5,   6,   7,   8,  -2,  -2,  -2,  -2,  -2,  -2,
  15,  16,  17,  18,  19,   0,  21,  26,   0,   0,
  27,  20 };
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
			
case 1:
# line 19 "cpy.y"
{return(yypvt[-1]);} break;
case 2:
# line 23 "cpy.y"
{yyval = yypvt[-2] * yypvt[-0];} break;
case 3:
# line 25 "cpy.y"
{yyval = yypvt[-2] / yypvt[-0];} break;
case 4:
# line 27 "cpy.y"
{yyval = yypvt[-2] % yypvt[-0];} break;
case 5:
# line 29 "cpy.y"
{yyval = yypvt[-2] + yypvt[-0];} break;
case 6:
# line 31 "cpy.y"
{yyval = yypvt[-2] - yypvt[-0];} break;
case 7:
# line 33 "cpy.y"
{yyval = yypvt[-2] << yypvt[-0];} break;
case 8:
# line 35 "cpy.y"
{yyval = yypvt[-2] >> yypvt[-0];} break;
case 9:
# line 37 "cpy.y"
{yyval = yypvt[-2] < yypvt[-0];} break;
case 10:
# line 39 "cpy.y"
{yyval = yypvt[-2] > yypvt[-0];} break;
case 11:
# line 41 "cpy.y"
{yyval = yypvt[-2] <= yypvt[-0];} break;
case 12:
# line 43 "cpy.y"
{yyval = yypvt[-2] >= yypvt[-0];} break;
case 13:
# line 45 "cpy.y"
{yyval = yypvt[-2] == yypvt[-0];} break;
case 14:
# line 47 "cpy.y"
{yyval = yypvt[-2] != yypvt[-0];} break;
case 15:
# line 49 "cpy.y"
{yyval = yypvt[-2] & yypvt[-0];} break;
case 16:
# line 51 "cpy.y"
{yyval = yypvt[-2] ^ yypvt[-0];} break;
case 17:
# line 53 "cpy.y"
{yyval = yypvt[-2] | yypvt[-0];} break;
case 18:
# line 55 "cpy.y"
{yyval = yypvt[-2] && yypvt[-0];} break;
case 19:
# line 57 "cpy.y"
{yyval = yypvt[-2] || yypvt[-0];} break;
case 20:
# line 59 "cpy.y"
{yyval = yypvt[-4] ? yypvt[-2] : yypvt[-0];} break;
case 21:
# line 61 "cpy.y"
{yyval = yypvt[-0];} break;
case 22:
# line 63 "cpy.y"
{yyval = yypvt[-0];} break;
case 23:
# line 66 "cpy.y"
{yyval = -yypvt[-0];} break;
case 24:
# line 68 "cpy.y"
{yyval = !yypvt[-0];} break;
case 25:
# line 70 "cpy.y"
{yyval = ~yypvt[-0];} break;
case 26:
# line 72 "cpy.y"
{yyval = yypvt[-1];} break;
case 27:
# line 74 "cpy.y"
{yyval= yypvt[-1];} break;
case 28:
# line 76 "cpy.y"
{yyval = yypvt[-0];} break;
case 29:
# line 78 "cpy.y"
{yyval= yypvt[-0];} break;
		}
		goto yystack;  /* stack new state and value */

	}
