#define ALLOC 127
#define ESIZE	256
#define PATSIZE	100

char *salloc();

sindex(p, s)
char *p, *s;
{
	register i, j;

	for(i = 0; p[i]; ++i)
		for(j = 0; s[j]; ++j)
			if(p[i] == s[j])
				return ++i;
	return 0;
}

char *trans(s, f, t)
char *s, *f, *t;
{
	char *ret;
	register char *tmp;
	register i, j;
	int t_len = strlen(t);

	ret = tmp = salloc(strlen(s), 0);
	for(i = 0; s[i]; i++) {
		for(j = 0; f[j]; ++j) {
			if(s[i] == f[j]) {
				if(j < t_len)
					*tmp++ = t[j];
				goto brk;
			}
		}
		*tmp++ = s[i];
brk:		;
	}
	*tmp = '\0';
	return ret;
}

char *substr(s, f, w)
register char *s;
register w;
{
	register char *sc;
	int sz = strlen(s);
	char *sr;

	if(f <= 0 || f > sz)
		return "\0";
	--f;
	w = w > (sz - f)? sz - f: w;
	s += f;
	sr = sc = salloc(w, 0);
	do
		*sc++ = *s++;
	while(--w);
	*sc = '\0';
	return sr;
}

char	*Mstring[10];
extern int	nbra;

#define INIT	register char *sp = instring;
#define GETC()		(*sp++)
#define PEEKC()		(*sp)
#define UNGETC(c)	(--sp)
#define RETURN(c)	return
#define ERROR(c)	errxx(c)

char	*loc1, *loc2, *locs;

ematch(s, p)
char *s;
register char *p;
{
	static char expbuf[ESIZE], last[PATSIZE];
	char *compile(), *salloc();
	register int i, num;
	extern char *braslist[], *braelist[];

	if(strcmp(p, last) != 0) {
		compile(p, expbuf, &expbuf[512], 0);
		strcpy(last, p);
	}
	if(advance(s, expbuf)) {
		for(i = nbra; i-- > 0;) {
			p = braslist[i];
			num = braelist[i] - p;
			strncpy(Mstring[i] = salloc(num + 1, 0), p, num);
			Mstring[i][num] = '\0';
		}
		return(loc2-s);
	}
	return(0);
}

errxx(c)
{
	error("RE error");
}

#include  "regexp.h"
