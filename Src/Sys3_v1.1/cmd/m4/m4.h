#define EOS	'\0'
#define LOW7	0177
#define MAXSYM	5
#define PUSH	1
#define NOPUSH	0
#define OK	0
#define NOT_OK	1

#define SPACE	1
#define DIG	2
#define ALPH	4
#define isspace(c)	(type[c]&SPACE)
#define isdigit(c)	(type[c]&DIG)
#define isalpha(c)	(type[c]&ALPH)
#define alphanum(c)	(type[c]&(ALPH|DIG))

#define	getchr()	(ip>ipflr?*--ip:\
	((C=(feof(ifile[ifx])?EOF:getc(ifile[ifx])))=='\n'?(fline[ifx]++,C):C))
#define	putbak(c)	(ip < ibuflm? (*ip++ = (c)): error(pbmsg,bufsize))
#define	stkchr(c)	(op < obuflm? (*op++ = (c)): error(aofmsg,bufsize))
#define sputchr(c,f)	(putc(c,f)=='\n'? lnsync(f): 0)
#define putchr(c)	(Cp?stkchr(c):cf?(sflag?sputchr(c,cf):putc(c,cf)):0)

struct bs {
	int	(*bfunc)();
	char	*bname;
};

struct	call {
	char	**argp;
	int	plev;
};

struct	nlist {
	char	*name;
	char	*def;
	char	tflag;
	struct	nlist *next;
};

extern FILE	*cf;
extern FILE	*ifile[];
extern FILE	*ofile[];
extern FILE	*xfopen();
extern char	**Ap;
extern char	**argstk;
extern char	*Wrapstr;
extern char	*astklm;
extern char	*calloc();
extern char	*copy();
extern char	*fname[];
extern char	*ibuf;
extern char	*ibuflm;
extern char	*ip;
extern char	*ipflr;
extern char	*ipstk[10];
extern char	*obuf;
extern char	*obuflm;
extern char	*op;
extern char	*procnam;
extern char	*tempname;
extern char	*token;
extern char	*toklm;
extern char	C;
extern char	aofmsg[];
extern char	astkof[];
extern char	badfile[];
extern char	fnbuf[];
extern char	lcom[];
extern char	lquote[];
extern char	nocore[];
extern char	nullstr[];
extern char	pbmsg[];
extern char	rcom[];
extern char	rquote[];
extern char	type[];
extern int	bufsize;
extern int	catchsig();
extern int	fline[];
extern int	hshsize;
extern int	hshval;
extern int	ifx;
extern int	nflag;
extern int	ofx;
extern int	pid;
extern int	sflag;
extern int	stksize;
extern int	sysrval;
extern int	toksize;
extern int	trace;
extern long	ctol();
extern struct bs	barray[];
extern struct call	*Cp;
extern struct call	*callst;
extern struct nlist	**hshtab;
extern struct nlist	*install();
extern struct nlist	*lookup();
