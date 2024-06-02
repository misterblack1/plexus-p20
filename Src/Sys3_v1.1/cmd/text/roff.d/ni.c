#include "tdef.hd"
#include "strs.hd"

/*	Filenames 	*/

char newf[NS];

#ifdef unix
struct	{
	char buf[NS];
	}  nextf[NSN] =	{
		"/usr/lib/tmac/tmac.",
		"/usr/lib/tmac/tmac.",
		"/usr/lib/tmac/tmac.",
		"/usr/lib/tmac/tmac."	};
int nfi = 19;
char suftab[] = "/usr/lib/suftab";
#ifdef NROFF
char termtab[NS] = "/usr/lib/term/tab";
int tti = 17;

#ifndef NOCOMPACT
char cmpctf[40] = "/usr/lib/macros/cmp.n.d.";
char cmpctuf[40] = "/usr/lib/macros/ucmp.n.";
#endif

#else

char fontfile[40] = "/usr/lib/font/ft";
int ffi = 16;

#ifndef NOCOMPACT
char cmpctf[40] = "/usr/lib/macros/cmp.t.d.";
char cmpctuf[40] = "/usr/lib/macros/ucmp.t.";
#endif

#endif
#endif

#ifdef tso

#ifdef NROFF
char termtab[NS] = "\164\141\142";	/*tab*/
int tti = 3;
#else
char fontfile[40] = "\146\164";	/*ft*/
int ffi = 2;
#endif

struct	{
	char buf[NS];
	}	nextf[NSN]  =  {
		"\164\155\141\143\056",	/*tmac.*/
		"\164\155\141\143\056",
		"\164\155\141\143\056",
		"\164\155\141\143\056",	};
int nfi = 5;

#endif

char obuf[OBUFSZ];
char *obufp = obuf;
int pto = 10000;
int pfrom = 1;
int print = 1;
#ifndef NROFF
int oldbits = -1;
int lg = LG;
#endif
int init = 1;
int fc = IMP;
int eschar = '\\';
int pl = 11*INCH;
int po = PO;
int dfact = 1;
int dfactd = 1;
int res = 1;
int smnt = 4;
int ascii = ASCII;
filedes ptid = PTID;
char ptname[] = "/dev/cat";
int pnlist[NPN] = {-1};
int *pnp = pnlist;
int npn = 1;
int npnflg = 1;
int xflg = 1;
int dpn = -1;
int totout = 1;
int ulfont = 1;
int ulbit = 1<<9;
int tabch = TAB;
int ldrch = LEADER;
int cd = 0;

/*	troff environment block	*/

#ifndef INCORE
struct envblock eblock =	{	/* environment initialization */
#else
struct envblock eblock[NEV] =	{
#endif
	/* Eics */	ICS,
	/* Eic */	0,
	/* Eicf */	0,
	/* Echbits */	0,
	/* Enmbits */	0,
	/* Eapts */	PS,
	/* Eapts1 */	PS,
	/* Epts */	PS,
	/* Epts1 */	PS,
	/* Efont */	FT,
	/* Efont1 */	FT,
	/* Esps */	SPS,
#ifndef NROFF
	/* Espacesz */	SS,
	/* Espbits */	0,
#endif
	/* Elss */	VS,
	/* Elss1 */	VS,
	/* Els */	1,
	/* Els1 */	1,
	/* Ell */	LL,
	/* Ell1 */	LL,
	/* Elt */	LL,
	/* Elt1 */	LL,
	/* Ead */	1,
	/* Enms */	1,
	/* Endf */	1,
	/* Efi */	1,
	/* Ecc */	'.',
	/* Ec2 */	'\'',
	/* Eohc */	OHC,
	/* Ehyf */	0,
	/* Ehyoff */	0,
	/* Eun1 */	-1,
	/* Etabc */	0,
	/* Edotc */	'.',
	/* Eadsp */	0,
	/* Eadrem */	0,
	/* Elastl */	0,
	/* Enel */	0,
	/* Eadmod */	0,
	/* *Ewordp */	0,
	/* Espflg */	0,
	/* *Elinep */	0,
	/* *Ewdend */	0,
	/* *Ewdstart */	0,
	/* Ewne */	0,
	/* Ene */	0,
	/* Enc */	0,
	/* Enb */	0,
	/* Elnmod */	0,
	/* Enwd */	0,
	/* Enn */	0,
	/* Eni */	0,
	/* Eul */	0,
	/* Ecu */	0,
	/* Ece */	0,
	/* Ein */	0,
	/* Ein1 */	0,
	/* Eun */	0,
	/* Ewch */	0,
	/* Ependt */	0,
	/* *Ependw */	0,
	/* Ependnf */	0,
	/* Espread */	0,
	/* Eit */	0,
	/* Eitmac */	0,
	/* Elnsize */	LNSIZE,
	/* *Ehyptr[NHYP] */	{0},
	/* Etabtab[NTAB] */	{DTAB*1, DTAB*2, DTAB*3, DTAB*4, DTAB*5, DTAB*6,
				 DTAB*7, DTAB*8, DTAB*9, DTAB*10, DTAB*11,
				 DTAB*12, DTAB*13, DTAB*14, DTAB*15, 0},
	/* Eline[LNSIZE] */	{0},
	/* Eword[WDSIZE] */	{0}
				};	/* end of environment initialization */

int oline[LNSIZE+1];
int ch_CMASK;	/* synonym for getch()&CMASK */
int tch[2] = {IMP, 0};		/* single character buffer */

extern caseds(), caseas(), casesp(), caseft(), caseps(), casevs(),
casenr(), caseif(), casepo(), casetl(), casetm(), casebp(), casech(),
casepn(), tbreak(), caseti(), casene(), casenf(), casece(), casefi(),
casein(), casell(), casens(), casemk(), casert(), caseam(),
casede(), casedi(), caseda(), casewh(), casedt(), caseit(), caserm(),
casern(), casead(), casers(), casena(), casepl(), caseta(), casetr(),
caseul(), caselt(), casenx(), caseso(), caseig(), casetc(), casefc(),
caseec(), caseeo(), caselc(), caseev(), caserd(), caseab(), casefl(),
done(), casess(), casefp(), casecs(), casebd(), caselg(), casehc(),
casehy(), casenh(), casenm(), casenn(), casesv(), caseos(), casels(),
casecc(), casec2(), caseem(), caseaf(), casehw(), casemc(), casepm(),
casecu(), casepi(), caserr(), caseuf(), caseie(), caseel(), casepc(),
caseht(), caseco(), caseunix();


/*	The following is a definition of the nroff/troff data area
 *	as saved by the compacted macros.  This includes name tables,
 *	trap tables, number register value tables, and translation
 *	tables.
 */


struct ddatablock dblock =	{	/* data block */

#include "nrtab.dt"
		0,		/* number register table */

#include "rqtab.dt"
		0,0		/* names table */

		};
