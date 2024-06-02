/*
 * lpd -- line printer daemon dispatcher
 *
 */

#include	<ctype.h>

#define	SPIDER	0
#define	PHONE	0
#define	LPD	1

char	dpd[]	= "/usr/spool/lpd";
char	dfname[30] = "/usr/spool/lpd/";
char	lock[]	= "/usr/spool/lpd/lock";

#include	"daemon.c"

/*
 * The remaining part is the line printer interface.
 */

char	lp[]	= "/dev/lp";
FILE	*lpf = NULL;

dem_con()
{
	return(0);
}

dem_dis()
{
	FCLOSE(lpf);
	FCLOSE(dfb);
}

dem_open(file)
char	*file;
{
	if((lpf = fopen(lp, "w")) == NULL)
		trouble("Can't open %s", lp);
}

dem_close()
{
	dem_dis();
}

get_snumb()
{
}

lwrite()
{
	banner(lpf, &line[1]);
}


FILE	*ibuf;

sascii(fc)
char	fc;
{
	if((ibuf = fopen(&line[1], "r")) == NULL)
		return(0);
	if(fc == 'F')
		putc(ff, lpf);
	bsopt();
	fflush(lpf);
	fclose(ibuf);
	return(0);
}

etcp1()
{
}


/*
 *	Copy file ibuf to lpf arranging that, to the extent possible,
 *	no backspaces will appear in the output.  Courtesy ARK
 */

/*
 *	Parameters.  BSIZE is the maximum permissible size for a line.
 *	Characters that would appear at a position in the line beyond
 *	BSIZE are quietly ignored.  DEPTH is the maximum number of characters
 *	that can be overstruck at a single character position before
 *	the program abandons optimization.  DEPTH should be at least
 *	3 to accomodate nroff gracefully.
 */
#define BSIZE 256
#ifndef	DEPTH
#define DEPTH 4
#endif

/*
 *	The following two arrays control buffering.  The four rows of b
 *	will contain text lines that should be overstruck on each other.
 *	The number of characters occupied in b[i] is given by size[i].
 *	If i > j, size[i] <= size[j].
 */
char b[DEPTH][BSIZE];
int size[DEPTH];

bsopt()
{
	register int cp, c, i;

	/* Initialization */
	cp = 0;
	for (i = 0; i < DEPTH; i++)
		size[i] = 0;

	/* Main loop, executed once per input character */
	while ((c = getc(ibuf)) != EOF) {
		if (isprint (c)) {
			if (cp < BSIZE) {
				/*
				 *	Try to find a buffer that can
				 *	contain this character.
				 */
				i = 0;
				while (i < DEPTH
				    && cp < size[i]
				    && b[i][cp] != ' ')
					i++;
				/*
				 *	If i is equal to DEPTH, all buffers
				 *	are full at this character position.
				 *	Force out the last buffer to make
				 *	room for the character.  Note that
				 *	this will probably make printer output
				 *	hideously slow (but correct!) if DEPTH
				 *	is too small.
				 */
				if (i == DEPTH) {
					i = DEPTH - 1;
					printline (i);
					putc ('\r', lpf);
				}
				/*
				 *	Expand this buffer with blanks up to cp.
				 */
				while (size[i] <= cp)
					b[i][size[i]++] = ' ';
				/*
				 *	Insert the character into the buffer
				 */
				b[i][cp++] = c;
			}
		} else {
			/*
			 *	Non-printing characters.  We ignore all except
			 *	space, backspace, new-line, return, tab,
			 *	and form feed.
			 */
			switch (c) {

			/*
			 *	Space: just increment the current position
			 *	on the line.
			 */
			case ' ':
				cp++;
				break;
			
			/*
			 *	Backspace: decrement the current position,
			 *	taking care to avoid backspacing off the
			 *	beginning of the line.
			 */
			case '\b':
				if (cp)
					cp--;
				break;

			/*
			 *	Tab: forward to the next tab stop (multiple
			 *	of 8).  This code assumes a 2-s complement
			 *	C implementation.
			 */
			case '\t':
				cp = (cp + 8) & -8;
				break;

			/*
			 *	Return: back to the beginning of the line
			 */
			case '\r':
				cp = 0;
				break;

			/*
			 *	Newline and formfeed: write out all the
			 *	buffers and then the input character.
			 */
			case '\n':
			case '\f':
				printall();
				putc(c, lpf);
				cp = 0;
				break;

			}
		}
	}
	/*
	 *	Dump out whatever is left, in case the input
	 *	doesn't end with a new-line.
	 */
	printall();
}

/*
 *	printall: print all the buffers, overstruck on each other.
 *	The cursor need not be left at the beginning of a line.
 */
printall()
{
	register int i, needcr;
	needcr = i = 0;
	while (i < DEPTH && size[i] > 0) {
		if (needcr)
			putc ('\r', lpf);
		printline (i);
		needcr = 1;
		i++;
	}
}

/*
 *	printline: print buffer number i, and set its size to zero.
 *	No return or newline character is put at the end.
 */
printline (i)
	register int i;
{
	register char *cp, *lim;

	cp = b[i];
	lim = cp + size[i];
	while (cp < lim)
		putc (*cp++, lpf);
	size[i] = 0;
}


/* VARARGS */
trouble(s, a1, a2, a3, a4)
char	*s;
{
	if(retcode != 0){
		dem_dis();
	}
	logerr(s, a1, a2, a3, a4);
	longjmp(env, 1);
}

/* VARARGS */
logerr(s, a1, a2, a3, a4)
char	*s;
int	a1, a2, a3, a4;
{
#ifdef DEBUG
	fprintf(stderr, s, a1, a2, a3, a4);
	putc('\n', stderr);
#endif
}

getowner()
{
}

maildname()
{
	fprintf(pmail, "Your %s job for file %s is finished.\n", DAEMNAM, mailfname);
}
