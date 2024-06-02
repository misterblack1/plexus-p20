#include	"crash.h"

struct	nlist	*stbl;
unsigned  symcnt;
extern	char	*namelist;

struct	nlist	*
search(addr, m1, m2)
	unsigned  addr;
	register  int  m1, m2;
{
	register  struct  nlist  *sp;
	register  struct  nlist  *save;
	unsigned	value;

	value = 0;
	save = 0;
	for(sp = stbl; sp != &stbl[symcnt]; sp++)
		if((sp->n_type == m1 || sp->n_type == m2) &&
			sp->n_value <= addr && sp->n_value > value) {
			value = sp->n_value;
			save = sp;
		}
	return(save);
}

struct	nlist	*
symsrch(s)
	register  char  *s;
{
	register  struct  nlist  *sp;
	register  int  i;
	int	found;

	found = 0;
	for(sp = stbl; sp != &stbl[symcnt]; sp++) {
		for(i = 0; i < 8; i++) {
			if(sp->n_name[i] == s[i])
				if(i == 7 || s[i] == '\0')
					found++;
				else
					continue;
			break;
		}
		if(found)
			break;
	}
	return(found ? sp : NULL);
}

struct	nlist	*
nmsrch(s)
	register  char  *s;
{
	char	ct[20];
	register  struct  nlist  *sp;

	if(strlen(s) > 19)
		return(0);
	if((sp = symsrch(s)) == NULL) {
		strcpy(ct, "_");
		strcat(ct, s);
		sp = symsrch(ct);
	}
	return(sp);
}

prnm(s)
	register  char  *s;
{
	register  char  *cp;
	register  struct  nlist  *sp;

	printf("%-10.10s ", s);
	if(strlen(s) > 8) {
		printf("bad name\n");
		return;
	}
	if((sp = nmsrch(s)) == NULL) {
		printf("no match\n");
		return;
	}
	printf("%7.7o  ", sp->n_value);
	switch(sp->n_type) {
	case 000: cp = "undefined"; break;
	case 001: cp = "absolute"; break;
	case 002: cp = "text"; break;
	case 003: cp = "data"; break;
	case 004: cp = "bss"; break;
	case 037: cp = "ld file name"; break;
	case 040: cp = "undefined"; break;
	case 041: cp = "absolute"; break;
	case 042: cp = "text"; break;
	case 043: cp = "data"; break;
	case 044: cp = "bss"; break;
	default:  cp = "unknown?"; break;
	}
	printf("%s\n", cp);
}

prod(addr, units, style)
	long	addr;
	int	units;
	char	*style;
{
	register  int  i;
	register  struct  prmode  *pp;
	int	word;
	char	ch;
	extern	struct	prmode	prm[];
	extern long int delta;

	if(units == -1)
		return;
	for(pp = prm; pp->pr_sw != 0; pp++)
		if(strcmp(pp->pr_name, style) == 0)
			break;
	lseek(kmem, addr + delta, 0);
	switch(pp->pr_sw) {
	default:
	case NULL:
		error("invalid mode");
		break;

	case OCTAL:
	case DECIMAL:
		if(addr & 01) {
			printf("warning: word allignment performed\n");
			addr &= ~01;
			lseek(kmem, addr + delta, 0);
		}
		for(i = 0; i < units; i++) {
			if(i % 8 == 0) {
				if(i != 0)
					putc('\n', stdout);
				printf("%7.7lo:", addr + i * sizeof (int));
			}
			if(read(kmem, &word, sizeof (int)) != sizeof (int)) {
				printf("  read error");
				break;
			}
			printf(pp->pr_sw == OCTAL ? " %7.7o" :
				"  %5u", word);
		}
		break;

	case CHAR:
	case BYTE:
		for(i = 0; i < units; i++) {
			if(i % (pp->pr_sw == CHAR ? 16 : 8) == 0) {
				if(i != 0)
					putc('\n', stdout);
				printf("%7.7lo: ", addr + i * sizeof (char));
			}
			if(read(kmem, &ch, sizeof (char)) != sizeof (char)) {
				printf("  read error");
				break;
			}
			if(pp->pr_sw == CHAR)
				putch(ch);
			else
				printf(" %4.4o", ch & 0377);
		}
		break;

	}
	putc('\n', stdout);
}
