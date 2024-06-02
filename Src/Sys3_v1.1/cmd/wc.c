/*
**	wc -- word and line count
*/

#include	<stdio.h>

char	b[512];

int	f = 0;
long	wordct;
long	twordct;
long	linect;
long	tlinect;
long	charct;
long	tcharct;

main(argc,argv)
char **argv;
{
	register char *p1, *p2;
	register int c;
	int	i, token;
	int	status = 0;
	char	*wd;

	wd = "lwc";
	if(argc > 1 && *argv[1] == '-') {
		wd = ++argv[1];
		argc--;
		argv++;
	}

	i = 1;
	do {
		if(argc>1 && (f=open(argv[i],0))<0) {
			fprintf(stderr, "wc: cannot open %s\n", argv[i]);
			status = 2;
			continue;
		}
		p1 = p2 = b;
		linect = 0;
		wordct = 0;
		charct = 0;
		token = 0;
		for(;;) {
			if(p1 >= p2) {
				p1 = b;
				c = read(f, p1, 512);
				if(c <= 0)
					break;
				charct += c;
				p2 = p1+c;
			}
			c = *p1++;
			if(' '<c&&c<0177) {
				if(!token) {
					wordct++;
					token++;
				}
				continue;
			}
			if(c=='\n')
				linect++;
			else if(c!=' '&&c!='\t')
				continue;
			token = 0;
		}

		/* print lines, words, chars */
		wcp(wd, charct, wordct, linect);
		if(argc>1) {
			printf(" %s\n", argv[i]);
		}
		else
			printf("\n");
		close(f);
		tlinect += linect;
		twordct += wordct;
		tcharct += charct;
	} while(++i<argc);
	if(argc > 2) {
		wcp(wd, tcharct, twordct, tlinect);
		printf(" total\n");
	}
	exit(status);
}

wcp(wd, charct, wordct, linect)
char *wd;
long charct; long wordct; long linect;
{
	register char *wdp=wd;

	while(*wdp) {
	switch(*wdp++) {
		case 'l':
			printf("%7ld", linect);
			break;

		case 'w':
			printf("%7ld", wordct);
			break;

		case 'c':
			printf("%7ld", charct);
			break;

		default:
			fprintf(stderr, "usage: wc [-clw] [name ...]\n");
			exit(2);
		}
	}
}
