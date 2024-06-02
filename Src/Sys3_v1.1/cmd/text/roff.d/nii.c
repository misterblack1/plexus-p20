#include "tdef.hd"
#ifdef NROFF
#include "tw.hd"
#endif
#include "strs.hd"
#ifdef unix
#include "sys/ioctl.h"
#include "termio.h"
#endif

struct s *frame, *stk, *ejl;
struct s *nxf;

#ifdef NROFF
#ifndef SMALL
int pipeflg;
#endif
int hflg;
int eqflg;
struct ttable t;		/* terminal table */
#endif

struct d d[NDI], *dip;		/* diversion information stack */

#ifndef NROFF
int xpts;
int verm;
int *pslp;
int psflg;
int ppts;
int pfont;
int paper;
int mpts;
int mfont;
int mcase;
int escm;
int cs;
int code;
int ccs;
int bd;
int back;
#endif

int level;
int stdi;
int waitf;
int nofeed;
int quiet;
int stop;
char ibuf[IBUFSZ];
char xbuf[IBUFSZ];
char *ibufp;
char *xbufp;
char *eibuf;
char *xeibuf;
int cbuf[NC];
int *cp;
int nx;
int mflg;
int ch = 0;
int cps;
int suffid;
#ifndef INCORE
union	{
	unsigned uns[26];
	char	chr[26*(sizeof(unsigned)/sizeof(char))];
	}  sufind;
#else
#include "suftab.c"
#endif
int ibf;
filedes ttyod;
#ifdef unix
struct termio ttys;
#endif
int iflg;
char *enda;
int rargc;
char **argp;
int lgf;
int copyf;
int cwidth;
struct tmpfaddr ip;
int nlflg;
int donef;
int nflush;
int nfo;
int ifile;
int padc;
int raw;
filedes ifl[NSO];
int ifi;
int flss;
int nonumb;
int trap;
int tflg;
int ejf;
int gflag;
int dilev;
int tlss;
struct tmpfaddr offset;
int em;
int ds;
struct tmpfaddr woff;
int app;
int ndone;
int lead;
int ralss;
struct tmpfaddr nextb;
int *argtop;
int nrbits;
int nform;
int oldmn;
int newmn;
int macerr;
struct tmpfaddr apptr;
int diflg;
int wbfi;
int evi;
int vflag;
int noscale;
int po1;
int evlist[EVLSZ];
int ev;
int tty;
int sfont;
int sv;
int esc;
int widthp;
int xfont;
int setwdf;
int xbitf;
int over;
int nhyp;
int **hyp;
int *olinep;
int esct;
int ttysave = -1;
int dotT;
char *unlkp;
int no_out;
int *cstk[CH0+1];		/* pointer stack for getch0 */
int cstkl;		/* cstk level */
int rptbuf[RPTLEN];	/* repeat char buffer */
