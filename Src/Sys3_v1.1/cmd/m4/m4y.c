
# line 2 "m4y.y"
extern long	evalval;
#define	YYSTYPE	long
# define DIGITS 257
# define OROR 258
# define ANDAND 259
# define GT 260
# define GE 261
# define LT 262
# define LE 263
# define NE 264
# define EQ 265
# define POWER 266
# define UMINUS 267
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

# line 48 "m4y.y"


extern char *pe;

yylex() {

	while (*pe==' ' || *pe=='\t' || *pe=='\n')
		pe++;
	switch(*pe) {
	case '\0':
	case '+':
	case '-':
	case '/':
	case '%':
	case '^':
	case '~':
	case '(':
	case ')':
		return(*pe++);
	case '*':
		return(peek('*', POWER, '*'));
	case '>':
		return(peek('=', GE, GT));
	case '<':
		return(peek('=', LE, LT));
	case '=':
		return(peek('=', EQ, EQ));
	case '|':
		return(peek('|', OROR, '|'));
	case '&':
		return(peek('&', ANDAND, '&'));
	case '!':
		return(peek('=', NE, '!'));
	default: {
		register	base;

		evalval = 0;

		if (*pe == '0') {
			if (*++pe=='x' || *pe=='X') {
				base = 16;
				++pe;
			} else
				base = 8;
		} else
			base = 10;

		for (;;) {
			register	c, dig;

			c = *pe;

			if (c>='0' && c<='9')
				dig = c - '0';
			else if (c>='a' && c<='f')
				dig = c - 'a' + 10;
			else if (c>='A' && c<='F')
				dig = c - 'A' + 10;
			else
				break;

			evalval = evalval*base + dig;
			++pe;
		}

		return(DIGITS);
	}
	}
}

peek(c, r1, r2)
{
	if (*++pe != c)
		return(r2);
	++pe;
	return(r1);
}

yyerror() {;}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 33,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	264, 0,
	265, 0,
	-2, 7,
-1, 34,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	264, 0,
	265, 0,
	-2, 8,
-1, 35,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	264, 0,
	265, 0,
	-2, 9,
-1, 36,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	264, 0,
	265, 0,
	-2, 10,
-1, 37,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	264, 0,
	265, 0,
	-2, 11,
-1, 38,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	264, 0,
	265, 0,
	-2, 12,
	};
# define YYNPROD 26
# define YYLAST 294
short yyact[]={

  24,  18,  25,   1,  48,  22,  20,   0,  21,   0,
  23,  24,  18,   0,   0,   0,  22,  20,   0,  21,
   0,  23,  24,  18,   0,   0,  24,  22,  20,   0,
  21,  22,  23,  24,  18,   0,  23,   0,  22,  20,
   0,  21,   0,  23,  24,  18,   0,   0,   0,  22,
  20,   0,  21,  24,  23,   0,   0,  19,  22,  20,
   0,  21,   0,  23,  24,   0,   3,   0,  19,  22,
  20,   0,  21,   5,  23,   0,   7,   0,   6,  19,
   0,   0,   0,   0,   0,   0,   0,  17,   0,   0,
  19,   0,   0,   0,   0,   0,   0,   0,  17,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  17,
   0,   2,   0,   0,   0,  26,  27,  28,  29,  30,
  17,  31,  32,  33,  34,  35,  36,  37,  38,  39,
  40,  41,  42,  43,  44,  45,  46,  47,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   9,  10,  13,  14,  15,  16,  12,  11,  25,
   0,   0,   9,  10,  13,  14,  15,  16,  12,  11,
  25,   0,   0,   0,  10,  13,  14,  15,  16,  12,
  11,  25,   0,   0,   0,  25,  13,  14,  15,  16,
  12,  11,  25,   0,   0,   0,   0,  13,  14,  15,
  16,  12,  11,  25,   0,   0,  13,  14,  15,  16,
  12,  11,  25,   0,   0,   0,   0,   0,   0,   0,
   8,   0,   0,  25 };
short yypact[]={

  33,-1000, -26,  33,  33,  33,  33,  33,-1000,  33,
  33,  33,  33,  33,  33,  33,  33,  33,  33,  33,
  33,  33,  33,  33,  33,  33,  16,  16, -37,-1000,
-1000, -15,  -4,  27,  27,  27,  27,  27,  27,   7,
  16,   7, -11, -11,-264,-264,-264,-264,-1000 };
short yypgo[]={

   0,   3, 111 };
short yyr1[]={

   0,   1,   1,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2 };
short yyr2[]={

   0,   1,   0,   3,   3,   2,   2,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
   3,   3,   3,   2,   2,   1 };
short yychk[]={

-1000,  -1,  -2,  33, 126,  40,  45,  43, 257, 258,
 259, 265, 264, 260, 261, 262, 263, 124,  38,  94,
  43,  45,  42,  47,  37, 266,  -2,  -2,  -2,  -2,
  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,
  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  41 };
short yydef[]={

   2,  -2,   1,   0,   0,   0,   0,   0,  25,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   5,   6,   0,  23,
  24,   3,   4,  -2,  -2,  -2,  -2,  -2,  -2,  13,
  14,  15,  16,  17,  18,  19,  20,  22,  21 };
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
# line 19 "m4y.y"
{ evalval = yypvt[-0]; } break;
case 2:
# line 20 "m4y.y"
{ evalval = 0; } break;
case 3:
# line 23 "m4y.y"
{ yyval = (yypvt[-2]!=0 || yypvt[-0]!=0) ? 1 : 0; } break;
case 4:
# line 24 "m4y.y"
{ yyval = (yypvt[-2]!=0 && yypvt[-0]!=0) ? 1 : 0; } break;
case 5:
# line 25 "m4y.y"
{ yyval = yypvt[-0] == 0; } break;
case 6:
# line 26 "m4y.y"
{ yyval = ~yypvt[-0]; } break;
case 7:
# line 27 "m4y.y"
{ yyval = yypvt[-2] == yypvt[-0]; } break;
case 8:
# line 28 "m4y.y"
{ yyval = yypvt[-2] != yypvt[-0]; } break;
case 9:
# line 29 "m4y.y"
{ yyval = yypvt[-2] > yypvt[-0]; } break;
case 10:
# line 30 "m4y.y"
{ yyval = yypvt[-2] >= yypvt[-0]; } break;
case 11:
# line 31 "m4y.y"
{ yyval = yypvt[-2] < yypvt[-0]; } break;
case 12:
# line 32 "m4y.y"
{ yyval = yypvt[-2] <= yypvt[-0]; } break;
case 13:
# line 33 "m4y.y"
{ yyval = (yypvt[-2]|yypvt[-0]); } break;
case 14:
# line 34 "m4y.y"
{ yyval = (yypvt[-2]&yypvt[-0]); } break;
case 15:
# line 35 "m4y.y"
{ yyval = (yypvt[-2]^yypvt[-0]); } break;
case 16:
# line 36 "m4y.y"
{ yyval = (yypvt[-2]+yypvt[-0]); } break;
case 17:
# line 37 "m4y.y"
{ yyval = (yypvt[-2]-yypvt[-0]); } break;
case 18:
# line 38 "m4y.y"
{ yyval = (yypvt[-2]*yypvt[-0]); } break;
case 19:
# line 39 "m4y.y"
{ yyval = (yypvt[-2]/yypvt[-0]); } break;
case 20:
# line 40 "m4y.y"
{ yyval = (yypvt[-2]%yypvt[-0]); } break;
case 21:
# line 41 "m4y.y"
{ yyval = (yypvt[-1]); } break;
case 22:
# line 42 "m4y.y"
{ for (yyval=1; yypvt[-0]-->0; yyval *= yypvt[-2]); } break;
case 23:
# line 43 "m4y.y"
{ yyval = yypvt[-0]-1; yyval = -yypvt[-0]; } break;
case 24:
# line 44 "m4y.y"
{ yyval = yypvt[-0]-1; yyval = yypvt[-0]; } break;
case 25:
# line 45 "m4y.y"
{ yyval = evalval; } break;
		}
		goto yystack;  /* stack new state and value */

	}
