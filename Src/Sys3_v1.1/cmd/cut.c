#
/* cut : cut and paste columns of a table (projection of a relation)  (GWRL) */
/* Release 1.5; handles single backspaces as produced by nroff    */
# include <stdio.h>	/* make: cc cut.c */
# define NFIELDS 512	/* max no of fields or resulting line length */
# define BACKSPACE 8
main(argc, argv)
int argc; char **argv;
{
	int del = '\t';
	int i, j, count, poscnt, r, s, t;
	int endflag, supflag, cflag, fflag, backflag, filenr;
	int sel[NFIELDS];
	register int c;
	register char *p1;
	char *p2, outbuf[NFIELDS];
	FILE *inptr;
 
 
while (argc > 1 && argv[1][0] == '-'){
	for (i = 1; (c = argv[1][i]) != '\0'; i++) {
		switch(c) {
			case 'd' : del = argv[1][++i];
				if (del == '\0') diag("no delimiter\n");
				break;
			case 's': supflag++ ;
				break;
			case 'c': cflag++ ;
				break;
			case 'f': fflag++ ;
				break;
			default : diag("Usage: cut [-s] [-d<char>] {-c<list> | -f<list>} file ...\n");
				break;
		}
		if (!endflag && (cflag || fflag)) {
			endflag = 1;
			r = s = t = 0;
			do {	c = argv[1][++i];
				switch(c) {
					case '-' : if (r) diagl();
						r = 1;
						if (t == 0)  s = 1;
						else {s = t; t = 0;}
						continue;
					case '\0' :
					case ','  : if (t >= NFIELDS) diagl();
						if (r) { if (t == 0) t = NFIELDS - 1;
							if (t<s) diagl();
							for(j = s; j <= t; j++) sel[j] = 1;
							}
						else sel[t] = (t > 0 ? 1 : 0);
						r = s = t = 0;
						if (c == '\0') {i--; break;}
						continue;
					default :
						if (c< '0' || c> '9') diagl();
						t = 10*t + c - '0';
						continue;
				}
				for (j = t = 0; j < NFIELDS; j++) t += sel[j];
				if (t == 0) diag("no fields\n");
			} while (c != '\0');
		}
	}
	--argc;
	++argv;
} /* end options */
if (!(cflag || fflag)) diagl();

--argc;
filenr = 1;
do {	/* for all input files */
	if (argc > 0) inptr = fopen(argv[filenr], "r");
	else inptr = stdin;
  
	if (inptr == NULL) {
		write(2,"Cannot open :",14);
		diag(argv[filenr]);
	}
	endflag = 0;
	do {	/* for all lines of a file */
		count = poscnt = backflag = 0;
		p1 = &outbuf[0] - 1 ;
		p2 = p1;
		do { 	/* for all char of the line */
			c = fgetc(inptr);
			if (c == EOF) {
				endflag = 1;
				break;
				}
			if (count == NFIELDS - 1) diag("line too long\n");
			if (c != '\n') *++p1 = c;
			if (cflag && (c == BACKSPACE)) backflag++ ; else 
				{ if ( !backflag ) poscnt += 1 ; else backflag-- ;}
			if ( backflag > 1 ) diag("cannot handle multiple adjacent backspaces\n");
			if ( ((c == '\n') && count > 0)  || c == del || cflag) {
				count += 1;
				if (fflag) poscnt = count  ;
				if (sel[poscnt]) p2 = p1; else p1 = p2;
			}
		}while (c != '\n');
		if ( !endflag && (count > 0 || !supflag)) {
			if (*p1 == del) *p1 = '\0';
				else *++p1 = '\0'; /*suppress trailing delimiter*/
			puts(outbuf);
			}
	} while (!endflag) ;
fclose(inptr);
} while(++filenr <= argc);
}

diag(s)
char *s;
{
	write(2, "cut : ", 6);
	while(*s)
		write(2,s++,1);
	exit(2);
}
diagl()
{
diag("bad list for c/f option\n");
}
