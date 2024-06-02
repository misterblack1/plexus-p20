#include "spell.h"

char notusedVRS[] = "@(#)spell.src	1.4";

char errmsg1[] = "spellout: arg count\n";
char errmsg2[] = "spellout: cannot initialize hash table\n";
main(argc, argv)
char **argv;
{
	register i, j;
	long h;
	register long *lp;
	char word[NW];
	int dflag = 0;
	int indict;
	register char *wp;

	if (argc>1 && argv[1][0]=='-' && argv[1][1]=='d') {
		dflag = 1;
		argc--;
		argv++;
	}
	if(argc<=1) {
		fwrite(errmsg1, sizeof(*errmsg1), sizeof(errmsg1), stderr);
		exit(1);
	}
	if(!prime(argc,argv)) {
		fwrite(errmsg2, sizeof(*errmsg2), sizeof(errmsg2), stderr);
		exit(1);
	}
	while (fgets(word, sizeof(word), stdin)) {
		indict = 1;
		for (i=0; i<NP; i++) {
			for (wp = word, h = 0, lp = pow2[i];
				(j = *wp) != '\0'; ++wp, ++lp)
				h += j * *lp;
			h %= p[i];
			if (get(h)==0) {
				indict = 0;
				break;
			}
		}
		if (dflag == indict)
			fputs(word, stdout);
	}
}
