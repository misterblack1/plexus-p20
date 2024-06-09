#define void int
/*
 * Sum bytes in file mod 2^16
 */

#define WDMSK 0177777L
#include <stdio.h>
#ifdef STOCKIII
#else
#undef BUFSIZ
#define BUFSIZ 1024
#endif
struct part {
	short unsigned hi,lo;
};
union hilo { /* this only works right in case short is 1/2 of long */
	struct part hl;
	long	lg;
} tempa, suma;

main(argc,argv)
char **argv;
{
	register unsigned sum;
	register i, c;
	register FILE *f;
	register long nbytes;
	int	alg, ca, errflg = 0;

	alg = 0;
	i = 1;
	if(argv[1][0]=='-' && argv[1][1]=='r') {
		alg = 1;
		i = 2;
	}

	do {
		if(i < argc) {
			if((f = fopen(argv[i], "r")) == NULL) {
				(void) fprintf(stderr, "sum: Can't open %s\n", argv[i]);
				errflg += 10;
				continue;
			}
		} else
			f = stdin;
		sum = 0;
		suma.lg = 0;
		nbytes = 0;
		if(alg == 1) {
			while((c = getc(f)) != EOF) {
				nbytes++;
				if(sum & 01)
					sum = (sum >> 1) + 0x8000;
				else
					sum >>= 1;
				sum += c;
				sum &= 0xFFFF;
			}
		} else {
			while((ca = getc(f)) != EOF) {
				nbytes++;
				suma.lg += ca & WDMSK;
			}
		}
		if(ferror(f)) {
			errflg++;
			(void) fprintf(stderr, "sum: read error on %s\n", argc>1?argv[i]:"-");
		}
		if (alg == 1)
			(void) printf("%05u%6ld", sum, (nbytes+BUFSIZ-1)/BUFSIZ);
		else {
			tempa.lg = (suma.hl.lo & WDMSK) + (suma.hl.hi & WDMSK);
			(void) printf("%u %ld", (unsigned)(tempa.hl.hi+tempa.hl.lo), (nbytes+BUFSIZ-1)/BUFSIZ);
		}
		if(argc > 1)
			(void) printf(" %s", argv[i]);
		(void) printf("\n");
		(void) fclose(f);
	} while(++i < argc);
	exit(errflg);
}
