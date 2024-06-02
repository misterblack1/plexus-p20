/*	@(#)/usr/src/cmd/make/files.c	3.4	*/

#include "defs"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <pwd.h>
#include <ar.h>
#include <a.out.h>
/* UNIX DEPENDENT PROCEDURES */


char archmem[16];
char archname[64];		/* name of archive library */


TIMETYPE exists(pname)
NAMEBLOCK pname;
{
	register CHARSTAR s;
	struct stat buf;
	TIMETYPE lookarch();
	CHARSTAR filename;

	filename = pname->namep;

	if(any(filename, LPAREN))
		return(lookarch(filename));

	if(stat(filename,&buf) < 0) 
	{
		s = findfl(filename);
		if(s != (CHARSTAR )-1)
		{
			pname->alias = copys(s);
			if(stat(pname->alias, &buf) == 0)
				return(buf.st_mtime);
		}
		return(0);
	}
	else
		return(buf.st_mtime);
}


TIMETYPE prestime()
{
	TIMETYPE t;
	time(&t);
	return(t);
}



FSTATIC char n15[15];
FSTATIC CHARSTAR n15end = &n15[14];



DEPBLOCK srchdir(pat, mkchain, nextdbl)
register CHARSTAR pat;		/* pattern to be matched in directory */
int mkchain;			/* nonzero if results to be remembered */
DEPBLOCK nextdbl;		/* final value for chain */
{
	FILE * dirf;
	int i, nread;
	CHARSTAR dirname, dirpref, endir, filepat, p;
	char temp[100];
	char fullname[100];
	CHARSTAR p1, p2;
	NAMEBLOCK q;
	DEPBLOCK thisdbl;
	OPENDIR od;
	PATTERN patp;

	struct direct entry[32];


	thisdbl = 0;

	if(mkchain == NO)
		for(patp=firstpat ; patp!=0 ; patp = patp->nextpattern)
			if(equal(pat,patp->patval))
				return(0);

	patp = ALLOC(pattern);
	patp->nextpattern = firstpat;
	firstpat = patp;
	patp->patval = copys(pat);

	endir = 0;

	for(p=pat; *p!=CNULL; ++p)
		if(*p==SLASH)
			endir = p;

	if(endir==0)
	{
		dirname = ".";
		dirpref = "";
		filepat = pat;
	}
	else
	{
		*endir = CNULL;
		dirpref = concat(pat, "/", temp);
		filepat = endir+1;
		dirname = temp;
	}

	dirf = NULL;

	for(od=firstod ; od!=0; od = od->nextopendir)
		if(equal(dirname, od->dirn))
		{
			dirf = od->dirfc;
			fseek(dirf,0L,0); /* start over at the beginning  */
			break;
		}

	if(dirf == NULL)
	{
		dirf = fopen(dirname, "r");
		od = ALLOC(opendir);
		od->nextopendir = firstod;
		firstod = od;
		od->dirfc = dirf;
		od->dirn = copys(dirname);
	}

	if(dirf == NULL)
	{
		fprintf(stderr, "Directory %s: ", dirname);
		fatal("Cannot open");
	}

	else	do
		{
			nread = fread(entry,sizeof(entry[0]),32,dirf) ;
			for(i=0; i<nread; ++i)
				if(entry[i].d_ino!= 0)
				{
					p1 = entry[i].d_name;
					p2 = n15;
					while( (p2<n15end) &&
					  (*p2++ = *p1++)!=CNULL );
					if( amatch(n15,filepat) )
					{
						concat(dirpref,n15,fullname);
						if( (q=srchname(fullname)) ==0)
							q = makename(copys(fullname));
						if(mkchain)
						{
							thisdbl = ALLOC(depblock);
							thisdbl->nextdep = nextdbl;
							thisdbl->depname = q;
							nextdbl = thisdbl;
						}
					}
				}
		} while(nread==32);

	if(endir != 0)
		*endir = SLASH;

	return(thisdbl);
}

/* stolen from glob through find */

amatch(s, p)
CHARSTAR s, p;
{
	register int cc, scc, k;
	int c, lc;

	scc = *s;
	lc = 077777;
	switch (c = *p)
	{

	case LSQUAR:
		k = 0;
		while (cc = *++p)
		{
			switch (cc)
			{

			case RSQUAR:
				if (k)
					return(amatch(++s, ++p));
				else
					return(0);

			case MINUS:
				k |= lc <= scc & scc <= (cc=p[1]);
			}
			if(scc==(lc=cc))
				k++;
		}
		return(0);

	case QUESTN:
	caseq:
		if(scc)
			return(amatch(++s, ++p));
		return(0);
	case STAR:
		return(umatch(s, ++p));
	case 0:
		return(!scc);
	}
	if(c==scc)
		goto caseq;
	return(0);
}

umatch(s, p)
register CHARSTAR s, p;
{
	if(*p==0)
		return(1);
	while(*s)
		if(amatch(s++,p))
			return(1);
	return(0);
}

#ifdef METERFILE
int meteron 0;	/* default: metering off */

meter(file)
CHARSTAR file;
{
	TIMETYPE tvec;
	CHARSTAR p, ctime();
	FILE * mout;
	struct passwd *pwd, *getpwuid();

	if(file==0 || meteron==0)
		return;

	pwd = getpwuid(getuid());

	time(&tvec);

	if( (mout=fopen(file,"a")) != NULL )
	{
		p = ctime(&tvec);
		p[16] = CNULL;
		fprintf(mout,"User %s, %s\n",pwd->pw_name,p+4);
		fclose(mout);
	}
}
#endif


/* look inside archives for notations a(b) and a((b))
	a(b)	is file member   b   in archive a
	a((b))	is entry point  _b  in object archive a
*/

static struct ar_hdr arhead;
FILE *arfd;
long int arpos, arlen;

static struct exec objhead;

static struct nlist objentry;


TIMETYPE lookarch(filename)
register CHARSTAR filename;
{
	register int i;
	CHARSTAR p, q, send;
	char s[15];
	int nc, nsym, objarch;

	for(p = filename; *p!= LPAREN ; ++p);
	q = p++;

	if(*p == LPAREN)
	{
		objarch = YES;
		nc = 8;
		++p;
	}
	else
	{
		objarch = NO;
		nc = 14;
		for(i = 0; i < 14; i++)
		{
			if(p[i] == RPAREN)
			{
				i--;
				break;
			}
			archmem[i] = p[i];
		}
		archmem[++i] = 0;
	}
	*q = CNULL;
	copstr(archname, filename);
	i = openarch(filename);
	*q = LPAREN;
	if(i == -1)
		return(0);
	send = s + nc;

	for( q = s ; q<send && *p!=CNULL && *p!=RPAREN ; *q++ = *p++ );

	while(q < send)
		*q++ = CNULL;
	while(getarch())
	{
		if(objarch)
		{
			getobj();
			nsym = objhead.a_syms / sizeof(objentry);
			for(i = 0; i<nsym ; ++i)
			{
				fread(&objentry, sizeof(objentry),1,arfd);
				if( (objentry.n_type & N_EXT)
				   && eqstr(objentry.n_name,s,nc))
				{
					for(i = 0; i < 14; i++)
						archmem[i] = arhead.ar_name[i];
					archmem[++i] = 0;
	out:
					clarch();
					return(arhead.ar_date);
				}
			}
		}

		else if( eqstr(arhead.ar_name, s, nc))
			goto out;
	}

	clarch();
	return( 0L);
}


clarch()
{
	fclose( arfd );
}


openarch(f)
register CHARSTAR f;
{
	int word = 0;
	struct stat buf;

	if(stat(f, &buf) == -1)
		return(-1);
	arlen = buf.st_size;

	arfd = fopen(f, "r");
	if(arfd == NULL)
		return(-1);
	fread(&word, sizeof(word), 1, arfd);
	if(word != ARMAG)
		fatal1("%s is not an archive", f);
/*
 *	trick getarch() into jumping to the first archive member.
 */
	arpos = sizeof(word);
	arhead.ar_size = -(int)sizeof(arhead);
	return(0);
}



getarch()
{
	arpos += sizeof(arhead);
	arpos += (arhead.ar_size + 1 ) & ~1L;
	if(arpos >= arlen)
		return(0);
	fseek(arfd, arpos, 0);
	fread(&arhead, sizeof(arhead), 1, arfd);
	return(1);
}


getobj()
{
	long int skip;

	fread(&objhead, sizeof(objhead), 1, arfd);
	if( objhead.a_magic != A_MAGIC1 &&
	    objhead.a_magic != A_MAGIC2 &&
	    objhead.a_magic != A_MAGIC3 )
			fatal1("%s is not an object module", arhead.ar_name);
	skip = objhead.a_text + objhead.a_data;
#ifdef vax
	skip += objhead.a_trsize + objhead.a_drsize;
#else
	if(! objhead.a_flag )
		skip *= 2;
#endif
	fseek(arfd, skip, 1);
}


eqstr(a,b,n)
register CHARSTAR a, b;
register int n;
{
	register int i;
	for(i = 0 ; i < n ; ++i)
		if(*a++ != *b++)
			return(NO);
	return(YES);
}
