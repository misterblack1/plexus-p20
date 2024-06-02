/* bits for testing */
#define _L	01
#define _U	02
#define _S	04
#define _EOF	020
#define _EOR	040
#define	_D	010

/* for single chars */
#define _HAT	0
#define _BAR	0
#define _BRA	0
#define _KET	0

#define _CBRA	0
#define _DOLLAR 0
#define _STAR	0
#define _EQ	0
#define _MINUS	0
#define _CKET	0
#define _PLUS	0
#define _COLON	0
#define _DOT	0
#define _QUERY	0

/* abbreviations for tests */
#define _ALPHANUM	(_ALPHA|_D)

char	_ctype[];
#define QUOTE	0200

/* nb these args are not call by value !!!! */
#define	space(c)	(((c)&QUOTE)==0 ANDF _ctype[c]&_S)
#define eolchar(c)	(((c)&QUOTE)==0 ANDF _ctype[c]&(_EOR|_EOF))
#define layout(c)	(((c)&QUOTE)==0 ANDF _ctype[c]&(_EOR|_EOF|_S))
#define	digit(c)	(((c)&QUOTE)==0 ANDF _ctype[c]&_D)
#define	letter(c)	(((c)&QUOTE)==0 ANDF _ctype[c]&_ALPHA)
#define	upper(c)	(((c)&QUOTE)==0 ANDF _ctype[c]&_U)
#define	lower(c)	(((c)&QUOTE)==0 ANDF _ctype[c]&_L)
#define alphanum(c)	(((c)&QUOTE)==0 ANDF _ctype[c]&_ALPHANUM)
