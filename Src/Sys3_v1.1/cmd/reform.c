/*
 *	reform: reformat file, esp. non-UNIX source. 0 of 4/24/77
 *	perform other manipulations needing meaning of tabs
 */
#define	NTABS	21	/* max # tab settings + 1 */
#define	NCOLS	512	/* max size of untabbed line */
#define	ESC	033

int tabin[NTABS],tabout[NTABS];
char	*tspec[2];		/* ptrs to tabspec1 & 2 */
#define TSPEC1	tspec[0]
#define TSPEC2	tspec[1]
int ntspec;		/* number of tabspecs found so far */

int	optb,	/* >0 ==> blank out 1st optb chars if blank/digits */
	opte,	/* >0 ==> extend length of each line to opte */
	optf,	/* >0 ==> write format line in front of file */
	opti = 1,	/* # of interior blanks required to make tab */
	optm,	/* >0 ==> move left optm characters */
	optp,	/* >0 ==> prepend optp characters */
	opts,	/* >0 ==> SCCS R.L moved to end of line */
	optt;	/* >0 ==> truncate lines to optt chars */

/*
 *	main working buffer, ptr to first and last (nl) char,
 *	ptrs to rightmost and leftmost legal positions
 */
char	line[2*NCOLS+1];
char	*pfirst, *plast;
char	*liner = &line[2*NCOLS],
	*linel = &line[0];
short	kludge = 0;	/* used when last line has no newline */
short	setkludge = 0;	/* " */
short	nl_last = 0;	/* " */


#define LINESZ1	132
char	line1[LINESZ1];	/* buffer area for 1st line1 in file */
char	found;	/* flag to note existence of format spec */
char	srelev[8];	/* if +s, save SCCS SID: chars* tab */
int	savem;	/* will have m from file fspec */
struct buf {
	int fildes;	/* file descriptor */
	int nleft;	/* bytes left */
	char *nextp;	/* ptr to next char */
	char buffer[512];	/* i/o area */
} fin, fout;

static char SCCSID[] = "@(#)reform.c	2.11";
main(argc,argv)
int argc; char **argv;
{
	get1line();	/* initialize input buffer with 1 line (at least) */
	fout.fildes = 1;
	fout.nextp = fout.buffer;
	fout.nleft = 0;
	options(argc,argv);
	if (optf)
		tswrite();	/* format spec into new file */
	while (getline(tabin) >= 0) {
		if (optp) prepend(optp);
		if (optt) truncate(optt);
		remtrail();
		if (opte) extend(opte);
		if (opts) sccsadd();
		if (optb) blankout(optb);
		if (optm) movleft(optm);
		contract(tabout,opti);
	}
	fflush(&fout);
	exit(0);
}

/*
 *	get1line: prime fin buffer with 1st line, for use with -- tabspecs.
 *	leaves fin so that getchal rereads 1st line
 */
get1line()
{
	register int i;
	register char *p;
	fin.nextp = p = &fin.buffer[0];
	for (i = 0; i <= 511;) {
		if (read(0, p, 1) != 1) break;
		i++;
		if (*p++ == '\n') break;
	}
	if((fin.nleft = i)==0)
		exit(0);
	return;
}

/*	options: scan option flags, setting flags opt? */
options(argc,argv)
int argc; char **argv;
{
	register char c;
	register int n;
	char *scan;
	while (--argc > 0) {
		scan = *++argv;		/* next arg, skipping 1st */
		if (*scan == '+') {
			c = *++scan;	/* option code */
			scan++;
			n = getnum(&scan);
			switch(c) {
			case 'b':
				optb = n?n:6; break;
			case 'e':
				opte = n?n:72; break;
			case 'f':
				optf++;
				break;
			case 'i':
				opti = n?n:2; break;
			case 'm':
				optm = n?n:6; break;
			case 'p':
				optp = n?n:6; break;
			case 's':
				opts++;
				if (!opte) opte = 72;
				break;
			case 't':
				optt = n?n:72; break;
			}
		}
		else if (ntspec < 2)
			tspec[ntspec++] = scan;	/* save tabspec */
	}
	if (!TSPEC1)
		TSPEC1 = "--";		/* 1st defaults to 1st line of file */
	if (!TSPEC2)
		TSPEC2 = TSPEC1;	/* but 2nd defaults to 1st tabspec */
	scantab(TSPEC1,tabin,0);
	if (opts)
		opte -= savem;
	scantab(TSPEC2,tabout,0);
	return;
}

/*
 *	functions scantab, repetab, arbitab, and getnum are identical to
 *	those in tabs(I).  filetab is similar, but saves 1st-line read
 *	and found flag externally, so that tswrite may use them
 */

/*	scantab: scan 1 tabspec & return tab list for it */

scantab(scan,tabvect,level)
char	*scan;
int tabvect[NTABS], level;
{
	register char c;
	if (*scan == '-')
		if ((c = *++scan) == '-')
			filetab(++scan,tabvect,level);
		else if (c >= '0' && c <= '9')
			repetab(scan,tabvect);
		else if (stdtab(scan,tabvect))
			err("unknown tab code");
		else;
	else
		arbitab(scan,tabvect);
	return;
}

/*	repetab: scan and set repetitive tabs, 1+n, 1+2*n, etc */

repetab(scan,tabvect)
char	*scan;
int tabvect[NTABS];
{
	register int incr, i, tabn;
	int limit;
	incr = getnum(&scan);
	tabn = 1;
	limit = (NCOLS - 1)/(incr?incr:1) - 1; /* # last actual tab */
	if (limit>NTABS-2)
		limit = NTABS-2;
	for (i = 0; i<=limit; i++)
		tabvect[i] = tabn += incr;
	tabvect[i] = 0;
	return;
}

/*	arbitab: handle list of arbitrary tabs */

arbitab(scan,tabvect)
char	*scan;
int tabvect[NTABS];
{
	register int i, t, last;
	char c;
	last = 0;
	for (i = 0; i<NTABS-1;) {
		if ((c = *scan) == '+') {
			scan++;		/* +n ==> increment, not absolute */
			if (t = getnum(&scan))
				tabvect[i++] = last += t;
			else err("illegal increment");
		}
		else {
			if ((t = getnum(&scan)) > last)
				tabvect[i++] = last = t;
			else err("illegal tabs");
		}
		if (*scan++ != ',') break;
	}
	if (last > NCOLS)
		err("illegal tabs");
	tabvect[i] = 0;
	return;
}

/*	filetab: copy tabspec from existing file */
filetab(scan,tabvect,level)
char	*scan;
int tabvect[NTABS];
{
	register int length, i;
	register char c;
	char *endspec;		/* ptr to blank or colon after tspec*/
	char savchar;
	int fildes;
	char state;
	char *temp;
	if (level)
		err("file indirection");
	if (*scan == '\0') {	/* tabspec was -- by itself, get from 1st line*/
		temp = &fin.buffer[0];
		if (opts) {
			while ((c = *temp++) != '\t' && c != '\n');
			if (c != '\t')
				err("not SCCS -m");
		}
		for (i = 0; i < LINESZ1;)
			line1[i++] = *temp++;	/* copy to safe place */
		length = LINESZ1;
	}
	else {
		if ((fildes = open(scan,0)) < 0)
			err("can't open");
		length = read(fildes,line1,LINESZ1);
		close(fildes);
	}
	found = state = 0;
	scan = 0;
	for (i = 0; i<length && (c = line1[i]) != '\n'; i++) {
		switch (state) {
		case 0:
			state = (c == '<'); break;
		case 1:
			state = (c == ':')?2:0; break;
		case 2:
		x:
			if (c == 't')
				state = 3;
			else if (c == ':')
				state = 6;
			else if (c != ' ') {
				if (c != 'm')
					state = 5;
				else {
					savem = line1[++i]-'0';
					c = line1[++i];
					goto x;
				}
			}
			break;
		case 3:
			if (c == ' ')
				state = 2;
			else {
				scan = &line1[i];
				if (*(scan+1) == 'c' && *(scan+2) == '3')
					savem = 6;
				state = 4;
			}
			break;
		case 4:
			if (c == ' ') {
				endspec = &line1[i];
				state = 5;
			}
			else if (c == ':') {
				endspec = &line1[i];
				state = 6;
			}
			break;
		case 5:
			if (c == 'm')
				goto x;
			if (c == ' ')
				state = 2;
			else if (c == ':')
				state = 6;
			break;
		case 6:
			if (c == '>') {
				found = 1;
				goto done;
			}
			else state = 5;
			break;
		}
	}
done:
	if (found && scan != 0) {
		savchar = *endspec;
				*endspec ='\0';
		scantab(scan,tabvect,1);
		*endspec = savchar;
	}
	else scantab("-8",tabvect,1);
	return;
}

/*
 *	tswrite: write format specification (tabout) as 1st line of file
 *	entry: if tabspec from --file, expects line1 and found to be set
 */

tswrite()
{
	if(TSPEC2[0] == '-' && TSPEC2[1] == '-')
		if (found)
			output(line1);
		else
			output("<:t-8 d:>\n");
	else {
		output("<:t");
		output(TSPEC2);
		output(" d:>\n");
	}
	return;
}

/*	getline: read next line into line, expand tabs into blanks */
/*	exit: return -1 on EOF, >= 0 otherwise */

getline(itab)
int itab[];
{
	char c;
	int i;		/* counter for sccs prefix */
	int ctab, pctab;
	register int nextcol;	/* logical position (allow BS/CTRL,etc*/
	register char *p;
	if (opts) {		/* if SCCS, stash SID away */
		for(i = 0; i <= 7; i++)
			srelev[i] = ' ';
		i = 0;
		while ((c = getchal()) != '\t') {
			if ((int)c == -1)
				return(-1);	/* EOF or error */
			if (c == '\n')
				err("not SCCS -m");
			if (i <= 7)
				srelev[i++] = c;
			else
				srelev[7] = '*';	/* overflow, mark */
		}
	}
	nextcol = 1;
	p = pfirst = line+NCOLS;	/* start at middle of workarea */
	do {
		if ((c = getchal()) < 0)
			break;
		if (c == '\t') {
			while ((ctab = *itab) && nextcol >= ctab) itab++;
			pctab = (int)(ctab ? p + (ctab-nextcol) : p+1);
			nextcol += pctab-(int)p;	/* adjust column for tab */
			while((int)p < pctab) *p++ = ' ';
		}
		else if ((*p++ = c) == '\n') break;
		if (c >= ' ') nextcol++;	/* all normal chars */
		else if (c == '\b' || c == ESC) nextcol--;
		/*	ESC+anything or ctrl char = 0-width */
	} while (p <= liner);
	plast = p-1;
	if (p > liner)
		err("line too long");
	return(c);
}

/*
 *	getchal: local variant of getchar with stop mode processing:
 *	make sure output flushed if last char was (nl), thus allowing
 *	nroff -s or .rd requests with small overhead.
 *	note EOF return is -1 rather than 0
 */
getchal()
{
	if (fin.nleft <= 0) {
		if (fin.nextp != 0 && *--fin.nextp == '\n')
			{ fflush(&fout); nl_last = 1; }	/* for kludge */
		else nl_last = 0;			/* for kludge */
		if (setkludge) {
			*fin.buffer = '\n';
			fin.nleft = 1;
			setkludge = 0;
			kludge = 1;
		}
		else
		fin.nleft = read(fin.fildes,fin.buffer,512);
		if ((fin.nleft < 512) && (kludge==0)) {	/* last line kludge */
			if (fin.nleft > 0) {
				if (*(fin.nextp + fin.nleft -1) != '\n')
					setkludge = 1; }
			else { if (nl_last == 0) { kludge = 1;
					fin.nleft = 1;
					*fin.buffer = '\n';
				}
			}
		}
		fin.nextp = fin.buffer;
	}
	if (fin.nleft-- > 0) return(*fin.nextp++);
	return(-1);
}

/*	prepend: add blanks to front of line */
prepend(n)
register int	n;
{
	register char *p;
	p = pfirst;
	if (pfirst < linel+n)
		err("line too long");
	while(n--) *--p = ' ';
	pfirst = p;
	return;
}

/*	truncate: truncate line to n chars max */
truncate(n)
register int	n;
{
	if (plast > pfirst+n)
		*(plast = pfirst+n) = '\n';
	return;
}

/*	remtrail: delete trailing blanks */
remtrail()
{
	register char *p;
	p = plast;
	while(*--p == ' ' && p >= pfirst);
	*++p = '\n';
	plast = p;
	return;
}

/*	blankout: blank first n chars, iff they are blanks/digits */
blankout(n)
register int	n;
{
	register char *p, *ptemp;
	char doblank;
	doblank = 1;
	ptemp = ((p = pfirst+n-1)<plast)?p:plast-1;
	for (p = pfirst; p <= ptemp && doblank; p++)
		if ((*p < '0' || *p > '9') && *p != ' ') doblank = 0;
	if (doblank)
		for(p = pfirst; p <= ptemp;) *p++ = ' ';
	return;
}

/*	extend: extend lines to fixed length */
extend(n)
register int	n;
{
	register char *p;
	register char *ptemp;
	p = plast;
	plast = ((ptemp = pfirst+n) < liner)?ptemp:liner-1;
	ptemp = plast;
	while (p < ptemp) *p++ = ' ';
	*p = '\n';
	return;
}

/*	movleft: shift line left (remove leading chars) */
movleft(n)
register int	n;
{
	pfirst = ((pfirst += n)<plast)?pfirst:plast;
	return;
}

/*
 *	sccsadd: add SCCS SID (release.level[etc...] onto end of line)
 */
sccsadd()
{
	register char *p;
	register int i;
	p = plast;		/* current loc of '\n' */
	for (i = 0; i <= 7; i++)
		if (srelev[i] != ' ')
			*p++ = srelev[i];
	*p = '\n';
	plast = p;
	return;
}

/*	contract: compress blanks into tabs and write output */
contract(itab,n)
int itab[], n;
{
	char outbuf[NCOLS];
	register char *p;	/* ptr to next char of input */
	register char *pout;	/* ptr to next empty slot of outbuf */
	register char c;
	int blnkcnt;	/* count of length of contiguous blank string */
	int nextcol;	/* next column # to be filled */
	int t, it;
	p = pfirst;
	pout = outbuf;
	blnkcnt = 0;
	nextcol = 1;
	do {
		while ((c = *p++) == ' ')
			blnkcnt++;
		if (blnkcnt) {	/* 1st nonblank after blank string */
			while (*itab && nextcol >= *itab) itab++;
			if (blnkcnt >= n || nextcol == 1) /* always lead blnks */
				while((it = *itab) && it <= nextcol+blnkcnt) {
					*pout++ = '\t';
					t = it-nextcol;
					blnkcnt -= t;
					nextcol += t;
					itab++;
				}
		}
		nextcol += blnkcnt;	/* leftover blanks, if any */
		while(blnkcnt--)
			*pout++ = ' ';
		blnkcnt = 0;
		*pout++ = c;	/* finally, actual input char */
		if (c >= ' ') nextcol++;	/* normal chars */
		else if (c == '\b' || c == ESC)
			nextcol--;
		/* ESC-anything or other ctrl chars = 0-width */
	} while(c != '\n');
	output(outbuf);
	return;
}

/*	output: write 1 line (terminated by '\0' or '\n') */
output(str)
register char	*str;
{
	register char c;
	while (c = *str++) {
		if (kludge && (c=='\n'))
			{kludge = 0; setkludge = 0; nl_last = 0; break; }
		putc(c,&fout);
		if (c == '\n') break;
	}
	return;
}

/*	err: terminate with error and print diagnostic */
err(mesg)
register char	*mesg;
{
	int ttydes,ttyold[3];
	char *p;
	if (gtty(ttydes = 2,ttyold)) ttydes = 1;
	write(ttydes,"reform:",7);
	p = mesg;
	while(*p++);
	write(ttydes,mesg,p-mesg);
	write(ttydes,"\n",1);
	exit(1);
}

/*
 *	getnum: scan and convert number, return zero if none found
 *	set scan ptr to addr of ending delimiter
 */
getnum(scan1)
char	**scan1;
{
	register int n;
	register char c, *scan;
	n = 0;
	scan = *scan1;
	while ((c = *scan++) >= '0' && c <= '9') n = n * 10 + c -'0';
	*scan1 = --scan;
	return(n);
}

/*
 *	stdtabs: standard tabs table
 *	format: option code letter(s), null, tabs, null
 */
char	stdtabs[] = {
'a',	0,1,10,16,36,72,0,			/* IBM 370 Assembler */
'a','2',0,1,10,16,40,72,0,			/* IBM Assembler alternative*/
'c',	0,1,8,12,16,20,55,0,			/* COBOL, normal */
'c','2',0,1,6,10,14,49,0,			/* COBOL, crunched*/
'c','3',0,1,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,67,0,
						/* crunched COBOL, many tabs */
'f',	0,1,7,11,15,19,23,0,			/* FORTRAN */
'p',	0,1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61,0, /* PL/I */
's',	0,1,10,55,0,				/* SNOBOL */
'u',	0,1,12,20,44,0,				/* UNIVAC ASM */
0};

/*
 *	stdtab: return tab list for any "canned" tab option.
 *	entry: option points to null-terminated option string
 *		tabvect points to vector to be filled in
 *	exit: return(0) if legal, tabvect filled, ending with zero
 *		return(-1) if unknown option
 */
stdtab(option,tabvect)
char	option[];
int tabvect[];
{
	register char *sp;
	tabvect[0] = 0;
	sp = stdtabs;
	while (*sp) {
		if (equal(option,sp)) {
			while (*sp++);		/* skip to 1st tab value */
			while (*tabvect++ = *sp++);	/* copy, make int */
			return(0);
		}
		while(*sp++);	/* skip to 1st tab value */
		while(*sp++);		/* skip over tab list */
	}
	return(-1);
}


/*	equal: string comparison, return 1 if equal, 0 otherwise */
equal(s1,s2)
char	*s1,*s2;
{
	register char c;
	while((c = *s1++) == *s2++ && c);
	if (c == *--s2)
		return(1);
	return(0);
}
putc(c, fid)
register char c;
register struct buf *fid;
{

	if(fid->nleft >= 512)
		fflush(fid);
	*fid->nextp++ = c;
	fid->nleft++;
}
fflush(fid)
register struct buf *fid;
{

	if(fid->nleft > 0)
		write(fid->fildes, fid->buffer, fid->nleft);
	fid->nleft = 0;
	fid->nextp = fid->buffer;
}
