	/* Lex source for program chek.  Flags questionalbe uses & errors 
	 * in usage of PWB/mm macros and some tbl and eqn constructs. */
C [.']
SP [ ]
	int i, max, il, jl ;   /* for list macros */
	char listm[20][20] ;  /* storage for list macro args */
	int ds, fs, ts, tsh, ns, eq, au, as ; /* use ds, as, ... as indicators of .DS, .AS, ... */
	int delset, ldelset, notwarned; /* used for EQN checks */
	char ldel, rdel;      /* used for EQN checks */
	int ae, tl, ok, bs ;
	int tm, nd, pla, ofl, tl ;


%%	/*	lex rules start	*/
	ds = fs = ts = tsh = ns = eq = au = as = ae = au = tl = ok = 0 ;
	tm = nd = pla = ofl = bs = 0 ;
	il = -1 ; /* list index, indexes listm 0-19 */
	delset = ldelset = notwarned = 0;

^{C}ND	{if(pla > 1)  /* pla is a place keeper for begin macro order */
	ofl = 1 ; /* ofl is an order error flag */
	else
	pla = 1 ;

	if(nd)
	printf("   Extra .ND at line %d\n",yylineno) ;

	if(pla == 9)
	printf("   .ND at line %d is after .MT\n",yylineno) ;
	nd = 1 ;
	}

^{C}TL	{if(pla>2)
	ofl = 1 ;
	else
	pla = 2 ;

	if(tl)
	printf("   Extra .TL at line %d\n",yylineno) ;

	if(pla == 9)
	printf("   .TL at line %d is after .MT\n",yylineno) ;
	tl = 1 ;
	}

^{C}AF	{if(pla>3)
	ofl = 1 ;
	else
	pla = 3 ;

	if(pla == 9)
	printf("   .AF at line %d is after .MT\n",yylineno) ;
	}

^{C}AU	{if(pla>4)
	ofl = 1 ;
	else
	pla = 4 ;

	if(pla == 9)
	printf("   .AU at line %d is after .MT\n",yylineno) ;
	au = 1 ;
	}

^{C}TM	{if(pla>5)
	ofl = 1 ;
	else
	pla = 5 ;

	if(tm)
	printf("   Extra .TM at line %d\n",yylineno) ;

	if(pla == 9)
	printf("   .TM at line %d is after .MT\n",yylineno) ;
	tl = 1 ;
	}

^{C}AS	{if(pla>6)
	ofl = 1 ;
	else
	pla = 6 ;

	if(as)
	printf("   Extra .AS at line %d\n",yylineno) ;

	if(pla == 9)
	printf("   .AS at line %d is after .MT\n",yylineno) ;
	as = 1 ;
	}

^{C}AE	{if(pla>7)
	ofl = 1 ;
	else
	pla = 7 ;

	if(ae)
	printf("   Extra .AE at line %d\n",yylineno) ;

	if(pla == 9)
	printf("   .AE at line %d is after .MT\n",yylineno) ;

	if(!as)
	printf("  Missing .AS before .AE at line %d\n",yylineno) ;
	ae = 1 ;
	}

^{C}OK	{if(pla>8)
	ofl = 1 ;
	else
	pla = 8 ;

	if(ok)
	printf("   Extra .OK at line %d\n",yylineno) ;

	if(pla == 9)
	printf("   .OK at line %d is after .MT\n",yylineno) ;
	ok = 1 ;
	}

^{C}MT	{if(ofl)
	printf("   Beginning macro sequence error before .MT at line %d\n",yylineno) ;

	if(pla == 9)
	printf("   Extra .MT at line %d\n",yylineno) ;
	pla = 9 ;

	if(!tl) /* .MT "requires" .TL &  .AU */
	printf("   No .TL before .MT at line %d\n",yylineno) ;

	if(!au)
	printf("   No .AU before .MT at line %d\n",yylineno) ;
	}

^{C}EQ {if (eq++)
	printf("   Spurious EQ, line %d\n", yylineno) ;

	if (ldelset)
	printf("   EQ in %c%c, line %d\n", ldel, rdel, yylineno) ;
	}

^{C}EN {if (eq==0)
	printf("   Spurious EN, line %d\n", yylineno) ;
	else
	eq = 0 ;

	if (ldelset) printf("   EN in %c%c, line %d\n", ldel, rdel, yylineno) ;
	}

^{SP}*define {if(ds && eq) {
	printf("   define at line %d inside .DS\n",yylineno) ; }
	}

^{SP}*delim{SP}*.. {if(eq && ds) 
	printf("   delim at line %d inside .DS\n",yylineno) ;

	if(eq && ds==0){
	if (ldelset) printf("   Missing right delimiter %c, at delimiter change\n", rdel);
	if (yytext[yyleng-2] == 'o' && yytext[yyleng-1] == 'f') {
	    if (delset == 0)
	    printf("   Spurious delim off, line %d\n", yylineno) ;
	    else
	    delset = 0 ;
	    ldelset = 0;
	    }
	else {
	    ldel = yytext[yyleng-2];
	    rdel = yytext[yyleng-1];
	    delset = 1;
	    ldelset = 0;
	    }
	}

	if(eq==0) REJECT;
	}

^{C}D(S|F)  {if(ds) {
     	printf("   .DS at line %d within .DS\n",yylineno) ; }

	if(as && ! ae) {
	printf("   .DS at line %d within .AS\n",yylineno) ; }

	if(fs) {
	printf("   .DS at line %d within .FS\n",yylineno) ; }
	ds = 1 ;
	}

^{C}FS {if(fs) {
	printf("   .FS at line %d within .FS\n",yylineno) ; }

	if(ds) {
	printf("   .FS at line %d within .DS\n",yylineno) ; }
	fs = 1 ;

	if(as && !ae) 
	printf("   .FS at line %d within .AS\n",yylineno) ;
	}

^{C}DE {if(ds == 0) {
	printf("   Extra .DE at line %d\n",yylineno) ; }

	if(fs) {
	printf("   .DE at line %d within .FS\n",yylineno) ; }

	if(as && !ae)
	printf("   .DE at line %d within .AS\n",yylineno) ;
	ds = 0 ;
	}

^{C}FE {if(fs == 0) {
	printf("   Extra .FE at line %d\n",yylineno) ; }

	if(as && !ae)
	printf("   .FE at line %d within .AS\n",yylineno) ;

	if(ds) {
	printf("   .FE at line %d within .DS\n",yylineno) ; }
	fs = 0 ; 
	}

^{C}LI	{if(il == -1)
	printf("   Missing list begin before .LI at line %d\n",yylineno) ;
	}

^{C}(AL|LB|DL|BL|RL|ML|VL).*$	{
	il++ ;

	if(il > 5 )
	printf("   Lists nested %d deep at line %d\n",il+1,yylineno) ;
	max = (yyleng < 20 ? yyleng : 19 ) ;
	for( jl = 0 ; jl < max ; jl++ )
	listm[il][jl] = yytext[jl] ;
	listm[il][jl] = '\0' ;
	}

^{C}SG 	{ if(fs)
	printf("   .SG at line %d within .FS\n",yylineno) ;

	if(ds)
	printf("   .SG at line %d within .DS\n",yylineno) ;

	if(!au )
	printf("   Missing .AU before .SG at line %d\n",yylineno) ;
	}

^{C}LE	{ if(--il < -1 )  {
	printf("   Extra .LE at line %d\n",yylineno) ;
	il = -1 ;
	}
	}

^{C}LC{SP}+[0-5][ \t]*$	{
	for( i = 4 ; yytext[i] < '0' || yytext[i] > '6' ; i++) ;
	i = yytext[i] - '0' ;

	if(i>il)
	printf("   .LC %d at line %d with %d lists active\n",i, yylineno, il + 1) ;
	else
	il = i - 1 ;
	}

^{C}LC.*$	{
	printf("   .LC syntax error at line %d\n",yylineno) ; }

^{C}NS {ns = 1 ; }

^{C}NE {if(ns == 0) {
	printf("   Missing .NS before .NE at line %d\n",yylineno) ; }
	ns = 0 ; 
	}

^{C}H {if(ts) {
	printf("   Missing .TE before .H at line %d\n",yylineno) ; }

	if(tsh) {
	printf("   Missing .TH before .H at line %d\n",yylineno) ; }

	if(ds) {
	printf("   Missing .DE before .H at line %d\n",yylineno) ; }

	if(fs) {
	printf("   Missing .FE before .H at line %d\n",yylineno) ; }

	if(ns) {
	printf("   Missing .NE before .H at line %d\n",yylineno) ; }

	if(eq) {
	printf("   Missing .EN before .H at line %d\n",yylineno) ; }

	if(as && !ae ) {
	printf("   Missing .AE before .H at line %d\n",yylineno) ; }
	while ( il > -1 ) {
	printf("   Missing .LE for \'%s\' before .H at line %d\n",
	listm[il--],yylineno) ; }
	ds = ns = fs = ts = eq = tsh = as = 0 ;
	il = -1 ;
	}

^{C}TS {if(ts) {
	printf("   .TS at line %d within .TS\n",yylineno) ; }
	ts = 1 ; 
	}

^{C}TS{SP}+H  {if(ts) {
	printf("   .TS at line %d within .TS\n",yylineno) ; }

	if(ds )
	printf("   .TS H at line %d within .DS\n",yylineno) ;
	tsh = 1 ;
	ts = 1 ;
	}

^{C}TE {if(ts == 0) {
	printf("   Extra .TE at line %d\n",yylineno) ; }

	if(tsh) {
	printf("   Missing .TH before .TE at line %d\n",yylineno) ; }
	ts = tsh = 0 ; 
	}

^{C}T& {if(ts == 0) {
	printf("   Missing .TS before .T& at line %d\n",yylineno) ; }
	}

^{C}TH {if(tsh == 0) {
	printf("   Missing .TS H before .TH at line %d\n",yylineno) ; }
	tsh = 0 ; 
	}

^{C}BS	{if(bs)
	printf("   .BS at line %d within .BS\n",yylineno) ;

	if(ds)
	printf("   .BS at line %d within .DS\n",yylineno) ;

	if(as && !ae)
	printf("   .BS at line %d within .AS\n",yylineno) ;

	if(fs)
	printf("   .BS at line %d within .FS\n",yylineno) ;

	bs = 1 ;
	}

^{C}.BE	{if(!bs)
	printf("   Missing .BS before .BE at line %d\n",yylineno) ;

	if(ds || fs )
	printf("   .BE at line %d within .DS or .FS\n",yylineno) ;

	if(as && !ae)
	printf("   .BE at line %d within .AS\n",yylineno) ;

	bs = 0 ;
	}

.	{if(delset){
	if(ldelset){
	 if (yytext[0] == rdel) {
  	    if (notwarned == 0) 
		  printf("    right delimiter %c, alone on line %d\n", rdel, yylineno);
	    ldelset = 0;
	 }
	}
	else {
	if (yytext[0] == ldel){
	    if (eq) printf("    left delimiter %c, in EQ, line %d\n", ldel, yylineno) ;
	    else { ldelset = 1; notwarned = 1; } 
	}
	}
	}
	}

\n	{if (ldelset && notwarned)
	printf("    left delimiter %c, alone on line %d\n", ldel, yylineno-1);
	notwarned = 0;
	}

%%	/*	lex rules end	*/

yywrap(){
	if(pla != 9 && pla > 0)
	printf("   Missing .MT ! !\n") ;

	if(ds) {
	printf("   Unfinished .DS\n") ; }

	if(bs)
	printf("   Unfinished .BS\n") ;

	if (eq) {
	printf("   Unfinished EQ\n") ; }

	if (ldelset) {
	printf("   Missing right delimiter %c\n", rdel); }

	if(fs) {
	printf("   Unfinished .FS\n") ; }

	if(ts) {
	printf("   Unfinished .TS\n") ; }

	if(ns) {
	printf("   Unfinished .NS\n") ; }

	if(as && !ae ) {
	printf("   Unfinished .AS\n") ; }

	if(tsh ) {
	printf("   Unfinished .TS H\n" ) ; }
	while ( il > -1 ) {
	printf("   Unfinished \'%s\'\n", listm[il--]) ; }
	printf("   %d lines done.\n\n",yylineno- 1) ;
	return(1) ;
}
