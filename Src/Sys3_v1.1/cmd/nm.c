/*	nm	COMPILE:	cc -O nm.c -s -i -o nm	*/

/*
**	print symbol tables for
**	object or archive files
**
**	nm [-gopruns] [name ...]
*/



#include	<ar.h>
#include	<a.out.h>
#include	<stdio.h>
#include	<ctype.h>

#define	MAGIC	exp.a_magic
#define	SELECT	arch_flg ? arp.ar_name : *argv

int	numsort_flg;
int	undef_flg;
int	revsort_flg = 1;
int	globl_flg;
int	nosort_flg;
int	arch_flg;
int	prep_flg;
int	size_flg;
int	sep_id;

struct	ar_hdr	arp;
struct	exec	exp;

FILE	*fi;

long	off;
long	ftell();

char	*malloc();
char	*realloc();

char stdbuf[BUFSIZ];

main(argc, argv)
char **argv;
{
	register	narg;
	int		mag;
	int		compare();

	setbuf (stdout, stdbuf);

	if(--argc>0 && argv[1][0]=='-' && argv[1][1]!=0) {
		argv++;
		while (*++*argv)
		switch (**argv) {
		case 'n':		/* sort numerically */
			numsort_flg++;
			continue;

		case 'g':		/* globl symbols only */
			globl_flg++;
			continue;

		case 'u':		/* undefined symbols only */
			undef_flg++;
			continue;

		case 'r':		/* sort in reverse order */
			revsort_flg = -1;
			continue;

		case 'p':		/* don't sort -- symbol table order */
			nosort_flg++;
			continue;

		case 'o':		/* prepend a name to each line */
			prep_flg++;
			continue;

		case 's':
			size_flg++;
			continue;

		default:		/* oops */
			fprintf(stderr, "usage: nm [-gnoprsu] [file ...]\n");
			exit(2);
		}
		argc--;
	}
	if(size_flg) {
		numsort_flg++;
		globl_flg++;
		undef_flg = 0;
		nosort_flg = 0;
	}
	if(argc == 0) {
		argc = 1;
		argv[1] = "a.out";
	}
	narg = argc;
	while(argc--) {
		fi = fopen(*++argv,"r");
		if(fi == NULL) {
			fprintf(stderr, "nm: cannot open %s\n", *argv);
			continue;
		}
		off = sizeof(mag);
		fread(&mag, 1, sizeof(mag), fi);/* try for an archive magic # */
		if(mag == ARMAG)
			arch_flg++;
		else {				/* try for an a.out magic # */
			rewind(fi);
			fread(&exp, 1, sizeof(exp), fi);
			if(BADMAG(exp)) {
				fprintf(stderr, "nm: %s-- bad format\n", *argv);
				continue;
			}
		}
		if(MAGIC == A_MAGIC3 && size_flg)	/* sep. I&D */
			sep_id++;
		rewind(fi);
		if(arch_flg) {
			nextel(fi);
			if (narg > 1)
				printf("\n%.14s:\n", *argv);
		}
		do {
			long o;
			register i, n, c;
			struct nlist *symp = NULL;
			struct nlist sym;

			fread(&exp, 1, sizeof(struct exec), fi);
			if(BADMAG(exp))		/* archive element not in  */
				continue;	/* proper format - skip it */
			o = (long)exp.a_text + exp.a_data;
#ifdef	vax
			o += (long)exp.a_trsize + exp.a_drsize;
#else
			if ((exp.a_flag & 01) == 0)
				o *= 2;
#endif
			fseek(fi, o, 1);
			n = exp.a_syms / sizeof(struct nlist);
			if(n == 0) {
				fprintf(stderr, "nm: %.14s -- no name list\n", SELECT);
				continue;
			}
			i = 0;
			while (--n >= 0) {
				fread(&sym, 1, sizeof(sym), fi);
				if(globl_flg && (sym.n_type&N_EXT)==0)
					continue;
				if (symp==NULL)
					symp = (struct nlist *)malloc(sizeof(struct nlist));
				else {
					symp = (struct nlist *)realloc(symp, (i+1)*sizeof(struct nlist));
				}
				if (symp == NULL) {
					fprintf(stderr, "nm: out of memory on %s\n", *argv);
					exit(2);
				}
				symp[i++] = sym;
			}
			if (nosort_flg==0) {
				qsort(symp, i, sizeof(struct nlist), compare);
				if(size_flg) {
					exp.a_data += exp.a_bss;
					for(n=0; n<i; n++) {
						if((c=symp[n+1].n_value-symp[n].n_value) < 0)
							c = -c;
						if(sep_id && symp[n+1].n_type == 'T') {
							c = exp.a_data - symp[n].n_value;
							sep_id = exp.a_data = 0;
						}
						if(n == i - 1)
							c = exp.a_text + exp.a_data - symp[n].n_value;
						symp[n].n_value = c;
					}
					qsort(symp, i, sizeof(struct nlist), compare);
				}
			}
			if ((arch_flg || narg>1) && prep_flg==0)
				printf("\n%s:\n", SELECT);
			for (n=0; n<i; n++) {
				register t;

				c = symp[n].n_type;
#ifdef	vax
				t = c & (N_TYPE - N_EXT);
#else
				t = c & N_TYPE;
#endif
				if(undef_flg && t != N_UNDF)
					continue;
				if (prep_flg) {
					if (arch_flg)
						printf("%s:", *argv);
					printf("%s:", SELECT);
				}
#ifdef	vax
				if (c & STABTYPE) {
					printf("%08x - %-8.8s %02x %02x %04x\n",
						symp[n].n_value,
						symp[n].n_name,
						symp[n].n_type & 0xff,
						symp[n].n_other & 0xff,
						symp[n].n_desc & 0xffff);
					continue;
				}
#endif
				switch (t) {
				case N_UNDF:
					c = 'u';
					if (symp[n].n_value)
						c = 'c';
					break;

#ifndef	vax
				default:
#endif
				case N_ABS:
					c = 'a';
					break;

				case N_TEXT:
					c = 't';
					break;

				case N_DATA:
					c = 'd';
					break;

				case N_BSS:
					c = 'b';
					break;

				case N_FN:
					c = 'f';
					break;

#ifndef	vax
				case N_REG:
					c = 'r';
					break;
#endif
				}
				if (symp[n].n_type&N_EXT)
					c = toupper(c);
				if (!undef_flg) {
					if (c=='u' || c=='U')
						printf("        ");
					else
						printf(FORMAT, symp[n].n_value);
					printf(" %c ", c);
				}
				printf("%.8s\n", symp[n].n_name);
		l1:;	}
			if (symp)
				free((char *)symp);
		} while(arch_flg && nextel(fi));
		fclose(fi);
	}
	exit(0);
}

compare(p1, p2)
struct nlist *p1, *p2;
{
	register i;

	if(sep_id) {
		if(p1->n_type == 'T' && p2->n_type != 'T')
			return(revsort_flg);
		if(p2->n_type == 'T' && p1->n_type != 'T')
			return(-revsort_flg);
	}
	if (numsort_flg) {
		if (p1->n_value > p2->n_value)
			return(revsort_flg);
		if (p1->n_value < p2->n_value)
			return(-revsort_flg);
	}
	for(i=0; i<sizeof(p1->n_name); i++)
		if (p1->n_name[i] != p2->n_name[i]) {
			if (p1->n_name[i] > p2->n_name[i])
				return(revsort_flg);
			else
				return(-revsort_flg);
		}
	return(0);
}

nextel(af)
FILE *af;
{
	register r;

	fseek(af, off, 0);
	r = fread((char *)&arp, 1, sizeof(struct ar_hdr), af);  /* read archive header */
	if (r <= 0)
		return(0);
	if (arp.ar_size & 1)
		++arp.ar_size;
	off = ftell(af) + arp.ar_size;	/* offset to next element */
	return(1);
}
