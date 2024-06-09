/*	@(#)/usr/src/cmd/make/doname.c	3.5	*/

#include "defs"


char Makecall;			/* flag which says whether to exec $(MAKE) */
extern char archmem[];
extern char archname[];

/*  BASIC PROCEDURE.  RECURSIVE.  */

/*
p->done = 0   don't know what to do yet
p->done = 1   file in process of being updated
p->done = 2   file already exists in current state
p->done = 3   file make failed
*/


doname(p, reclevel, tval)
register NAMEBLOCK p;
int reclevel;
TIMETYPE *tval;
{
	register DEPBLOCK q;
	register LINEBLOCK lp;
	int errstat;
	int okdel1;
	int didwork;
	TIMETYPE td, td1, tdep, ptime, ptime1;
	DEPBLOCK qtemp, suffp, suffp1;
	NAMEBLOCK p1, p2;
	SHBLOCK implcom, explcom;
	LINEBLOCK lp1, lp2;
	char sourcename[100],prefix[100],temp[100],concsuff[20];
	CHARSTAR pnamep, p1namep;
	CHAIN qchain;
	int found, onetime;
	CHARSTAR savenamep;

	if(p == 0)
	{
		*tval = 0;
		return(0);
	}

	if(IS_ON(DBUG))
	{
		blprt(reclevel);
		printf("doname(%s,%d)\n",p->namep,reclevel);
		fflush(stdout);
	}

	if(p->done > 0)
	{
		*tval = p->modtime;
		return(p->done == 3);
	}

	errstat = 0;
	tdep = 0;
	implcom = 0;
	explcom = 0;
	ptime = exists(p);
	if(reclevel == 0 && IS_ON(DBUG))
	{
		blprt(reclevel);
		printf("TIME(%s)=%ld\n", p->namep, ptime);
	}
	ptime1 = 0;
	didwork = NO;
	p->done = 1;	/* avoid infinite loops */

	qchain = NULL;

/*
 *	Perform runtime dependency translations.
 */
	if(p->rundep == 0)
	{
		setvar("@", p->namep);
		dynamicdep(p);
		setvar("@", 0);
	}

/*
 *	Expand any names that have embedded metacharaters. Must be
 *	done after dynamic dependencies because the dyndep symbols
 *	($(*D)) may contain shell meta characters.
 */
	expand(p);



/*
 *	FIRST SECTION -- GO THROUGH DEPENDENCIES
 */

	if(IS_ON(DBUG))
	{
		blprt(reclevel);
		printf("look for explicit deps. %d \n", reclevel);
	}
	for(lp = p->linep ; lp!=0 ; lp = lp->nextline)
	{
		td = 0;
		for(q = lp->depp ; q!=0 ; q=q->nextdep)
		{
			q->depname->backname = p;
			errstat += doname(q->depname, reclevel+1, &td1);
			if(IS_ON(DBUG))
			{
			    blprt(reclevel);
			    printf("TIME(%s)=%ld\n", q->depname->namep, td1);
			}
			td = max(td1,td);
			if(ptime < td1)
				appendq(&qchain, q->depname->namep);
		}
		if(p->septype == SOMEDEPS)
		{
			if(lp->shp!=0)
				if( ptime<td || (ptime==0 && td==0) || lp->depp==0)
				{
					okdel1 = okdel;
					okdel = NO;
					setvar("@", p->namep);
					if(savenamep)
						setvar("%", archmem);
					setvar("?", mkqlist(qchain) );
					qchain = NULL;
					if( IS_OFF(QUEST) )
					{
						ballbat(p, reclevel);
						errstat += docom(lp->shp);
					}
					setvar("@", 0);
					setvar("%", 0);
					okdel = okdel1;
					if( (ptime1 = exists(p)) == 0)
						ptime1 = prestime();
					didwork = YES;
				}
		}

		else
		{
			if(lp->shp != 0)
			{
				if(explcom)
					fprintf(stderr, "Too many command lines for `%s'\n",
						p->namep);
				else
					explcom = lp->shp;
			}

			tdep = max(tdep, td);
		}
	}

/*
 *	SECOND SECTION -- LOOK FOR IMPLICIT DEPENDENTS
 */

	if(IS_ON(DBUG))
	{
		blprt(reclevel);
		printf("look for implicit rules. %d \n", reclevel);
	}
	found = 0; onetime = 0;
	if(any(p->namep, LPAREN))
	{
		savenamep = p->namep;
		p->namep = copys(archmem);
		if(IS_ON(DBUG))
		{
			blprt(reclevel);
			printf("archmem = %s\n", archmem);
		}
		if(IS_ON(DBUG)) 
		{
			blprt(reclevel);
			printf("archname = %s\n", archname);
		}
	}
	else
		savenamep = 0;


	for(lp=sufflist ; lp!=0 ; lp = lp->nextline)
	for(suffp = lp->depp ; suffp!=0 ; suffp = suffp->nextdep)
	{
		pnamep = suffp->depname->namep;
		if(suffix(p->namep , pnamep , prefix))
		{
			if(IS_ON(DBUG)) 
			{
				blprt(reclevel);
				printf("right match = %s\n",p->namep);
			}
			found = 1;
			if(savenamep)
				pnamep = ".a";
searchdir:

			copstr(temp, prefix);
			addstars(temp);
			srchdir( temp , NO, NULL);
			for(lp1 = sufflist ; lp1!=0 ; lp1 = lp1->nextline)
			for(suffp1=lp1->depp ; suffp1!=0 ; suffp1 = suffp1->nextdep)
			{
				p1namep = suffp1->depname->namep;
				concat(p1namep, pnamep, concsuff);
				if( (p1=srchname(concsuff)) == 0)
					continue;
				if(p1->linep == 0)
					continue;
				concat(prefix, p1namep, sourcename);
				if(any(p1namep, WIGGLE))
				{
					sourcename[strlen(sourcename) - 1] = CNULL;
					if(is_sccs(sourcename) == NO)
						trysccs(sourcename);
				}
				if( (p2=srchname(sourcename)) == 0)
					continue;
				if(equal(sourcename, p->namep))
					continue;
/*
 *	FOUND -- left and right match
 */

				found = 2;
				if(IS_ON(DBUG))
				{
				  blprt(reclevel);
				  printf("%s ---%s--- %s\n",
					sourcename, concsuff, p->namep);
				}
				p2->backname = p;
				errstat += doname(p2, reclevel+1, &td);
				if(ptime < td)
					appendq(&qchain, p2->namep);
				if(IS_ON(DBUG))
				{
					blprt(reclevel);
					printf("TIME(%s)=%ld\n",p2->namep,td);
				}
				tdep = max(tdep, td);
				setvar("*", prefix);
				setvar("<", sourcename);
				for(lp2=p1->linep ; lp2!=0 ; lp2 = lp2->nextline)
					if(implcom = lp2->shp) break;
				goto endloop;
			}
/*
 *	quit search for single suffix rule.
 */
			if(onetime == 1)
				goto endloop;
		}
	}

endloop:


/*
 * look for a single suffix type rule.
 * only possible if no explicit dependents and no shell rules
 * are found, and nothing has been done so far. (previously, `make'
 * would exit with 'Don't know how to make ...' message.
 */
	if(found == 0)
	if(onetime == 0)
	if(	  p->linep == 0 ||
		( p->linep->depp == 0 && p->linep->shp == 0))
	{
		onetime = 1;
		if(IS_ON(DBUG))
		{
			blprt(reclevel);
			printf("Looking for Single suffix rule.\n");
		}
		concat(p->namep, "", prefix);
		pnamep = "";
		goto searchdir;
	}


/*
 *	THIRD SECTION -- LOOK FOR DEFAULT CONDITION OR DO COMMAND
 */
	if(errstat==0 && (ptime<tdep || (ptime==0 && tdep==0) ) )
	{
		if(savenamep)
		{
			setvar("@", archname);
			setvar("%", archmem);
		}
		else
		{
			setvar("@", p->namep);
		}
		setvar("?", mkqlist(qchain) );
		ballbat(p, reclevel);
		if(explcom)
			errstat += docom(explcom);
		else if(implcom)
			errstat += docom(implcom);
		else if( (p->septype != SOMEDEPS && IS_OFF(MH_DEP)) ||
			 (p->septype == 0        && IS_ON(MH_DEP) )    )
/*
 *	OLD WAY OF DOING TEST is
 *		else if(p->septype == 0)
 *	notice above, a flag has been put in to get the murray hill version.
 *	the flag is "-b" (for botch).
 */
			if(p1=srchname(".DEFAULT"))
			{
				if(IS_ON(DBUG))
				{
					blprt(reclevel);
					printf("look for DEFAULT rule. %d \n", reclevel);
				}
				setvar("<", p->namep);
				for(lp2=p1->linep ; lp2!=0 ; lp2 = lp2->nextline)
					if(implcom = lp2->shp)
					{
						errstat += docom(implcom);
					}
			}
			else if(IS_OFF(GET) ||
				  !get(p->namep, NOCD, 0) )
			{
				fatal1(" Don't know how to make %s", p->namep);
			}

		setvar("@", 0);
		setvar("%", 0);
		if(IS_ON(NOEX) || (ptime = exists(p)) == 0)
			ptime = prestime();
	}

	else if(errstat!=0 && reclevel==0)
		printf("`%s' not remade because of errors\n", p->namep);

	else if(IS_OFF(QUEST) && reclevel==0  &&  didwork==NO)
		printf("`%s' is up to date.\n", p->namep);

	if(IS_ON(QUEST) && reclevel==0)
		exit(ndocoms>0 ? -1 : 0);

	p->done = (errstat ? 3 : 2);
	ptime = max(ptime1, ptime);
	p->modtime = ptime;
	*tval = ptime;
	setvar("<", 0);
	setvar("*", 0);
	return(errstat);
}

docom(q)
SHBLOCK q;
{
	CHARSTAR s;
	int status;
	int ign, nopr;
	char string[OUTMAX];

	++ndocoms;
	if(IS_ON(QUEST))
		return(0);

	if(IS_ON(TOUCH))
	{
		s = varptr("@")->varval;
		if(IS_OFF(SIL))
			printf("touch(%s)\n", s);
		if(IS_OFF(NOEX))
			touch(s);
	}

	else for( status = 0; q!=0 ; q = q->nextsh )
	{
/*
 *	Allow recursive makes to execute only if the NOEX flag set
 */
		if(sindex(q->shbp, "$(MAKE)") != -1 && IS_ON(NOEX))
			Makecall = YES;
		else
			Makecall = NO;
		subst(q->shbp,string);

		ign = IS_ON(IGNERR) ? YES : NO;
		nopr = NO;
		for(s = string ; *s==MINUS || *s==AT ; ++s)
			if(*s == MINUS)  ign = YES;
			else nopr = YES;

		if( docom1(s, ign, nopr) && !ign)
			if(IS_ON(KEEPGO))
				return(1);
			else	fatal(0);
	}
	return(0);
}



docom1(comstring, nohalt, noprint)
register CHARSTAR comstring;
int nohalt, noprint;
{
	register int status;

	if(comstring[0] == '\0') return(0);

	if(IS_OFF(SIL) && (!noprint || IS_ON(NOEX)) )
	{
		CHARSTAR p1, ps;
		CHARSTAR pmt = prompt;

		ps = p1 = comstring;
		while(1)
		{
			while(*p1 && *p1 != NEWLINE) p1++;
			if(*p1)
			{
				*p1 = 0;
				printf("%s%s\n", pmt, ps);
				*p1 = NEWLINE;
				ps = p1 + 1;
				p1 = ps;
			}
			else
			{
				printf("%s%s\n", pmt, ps);
				break;
			}
		}

		fflush(stdout);
	}

	if( status = dosys(comstring, nohalt) )
	{
		if( status>>8 )
			printf("*** Error code %d", status>>8 );
		else	printf("*** Termination code %d", status );

		if(nohalt) printf(" (ignored)\n");
		else	printf("\n");
		fflush(stdout);
	}

	return(status);
}


/*
 *	If there are any Shell meta characters in the name,
 *	search the directory, and if the search finds something
 *	replace the dependency in "p"'s dependency chain. srchdir
 *	produces a DEPBLOCK chain whose last member has a null
 *	nextdep pointer or the NULL pointer if it finds nothing.
 *	The loops below do the following: for each dep in each line
 *	if the dep->depname has a shell metacharacter in it and
 *	if srchdir succeeds, replace the dep with the new one
 *	created by srchdir. The Nextdep variable is to skip over
 *	the new stuff inserted into the chain.
*/

expand(p)
NAMEBLOCK p;
{
	register DEPBLOCK db;
	register DEPBLOCK Nextdep;
	register CHARSTAR s;
	register DEPBLOCK srchdb;
	register LINEBLOCK lp;



	for(lp = p->linep ; lp!=0 ; lp = lp->nextline)
		for(db=lp->depp ; db!=0 ; db=Nextdep )
		{
			Nextdep = db->nextdep;
			if(any( (s=db->depname->namep), STAR) ||
			   any(s, QUESTN) || any(s, LSQUAR) )
				if( srchdb = srchdir(s , YES, NULL) )
					dbreplace(p, db, srchdb);
		}
}
/*
 *	Replace the odb depblock in np's dependency list with the
 *	dependency chain defined by ndb. This is just a linked list insert
 *	problem. dbreplace assumes the last "nextdep" pointer in
 *	"ndb" is null.
 */
dbreplace(np, odb, ndb)
register NAMEBLOCK np;
register DEPBLOCK odb, ndb;
{
	register LINEBLOCK lp;
	register DEPBLOCK  db;
	register DEPBLOCK  enddb;

	for(enddb = ndb; enddb->nextdep; enddb = enddb->nextdep);

	for(lp = np->linep; lp; lp = lp->nextline)
		if(lp->depp == odb)
		{
			enddb->nextdep	= lp->depp->nextdep;
			lp->depp	= ndb;
			return;
		}
		else
		{
			for(db = lp->depp; db; db = db->nextdep)
				if(db->nextdep == odb)
				{
					enddb->nextdep	= odb->nextdep;
					db->nextdep	= ndb;
					return;
				}
		}
}


#define NPREDS 50

ballbat(np, reclevel)
NAMEBLOCK np;
{
	static char ballb[200];
	register CHARSTAR p;
	register NAMEBLOCK npp;
	register int i;
	VARBLOCK vp;
	int npreds=0;
	NAMEBLOCK circles[NPREDS];


	if( (vp=varptr("!"))->varval == 0)
		vp->varval = ballb;
	p = ballb;
	p = copstr(p, varptr("<")->varval);
	p = copstr(p, " ");
	for(npp = np; npp; npp = npp->backname)
	{
		for(i = 0; i < npreds; i++)
		{
			if(npp == circles[i])
			{
				fprintf(stderr,"$! nulled, predecessor circle\n");
				ballb[0] = CNULL;
				return;
			}
		}
		circles[npreds++] = npp;
		if(npreds >= NPREDS)
		{
			fprintf(stderr, "$! nulled, too many predecessors\n");
			ballb[0] = CNULL;
			return;
		}
		p = copstr(p, npp->namep);
		p = copstr(p, " ");
	}
}

/*
 *	PRINT n BLANKS WHERE n IS THE CURRENT RECURSION LEVEL.
 */
blprt(n)
register int n;
{
	while(n--)
		printf("   ");
}
