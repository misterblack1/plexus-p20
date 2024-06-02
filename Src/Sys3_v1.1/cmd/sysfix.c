/*
 * fix system image for I/D space
 *  Move data down to 0; move text to 4K.
 *  Also put the data at the start of the
 *  file and the text after it.
 */

#include <stdio.h>
FILE *tbuf;
FILE *rbuf;
FILE *obuf;
long	txtsiz;
long	datsiz;
unsigned bsssiz;
unsigned symsiz;

char	utsname[9];
unsigned utsseek 0;
int	symbuf[6];
int	txtrel	8192;
int	datrel;
FILE *symfd;
char	symname[9];

main(argc, argv)
char **argv;
{
	register word, rel;
	register unsigned s;

	if (argc<3) {
		fprintf(stderr,"Usage: sysfix a.out unix [utsname]\n");
		exit(1);
	}
	if (argc==4)
		strncpy(utsname, argv[3], 8);
	if ((tbuf = fopen(argv[1], "r"))==NULL) {
		fprintf(stderr,"Cannot open input %s\n",argv[1]);
		exit(1);
	}
	rbuf = fopen(argv[1], "r");
	symfd = fopen(argv[1], "r");
	if ((obuf = fopen(argv[2], "w"))==NULL) {
		fprintf(stderr,"Cannot create output %s\n",argv[2]);
		exit(1);
	}
	if (getw(tbuf) != 0407) {
		fprintf(stderr,"Bad input format\n");
		exit(1);
	}
	txtsiz = (unsigned)getw(tbuf);
	datsiz = (unsigned)getw(tbuf);
	bsssiz = getw(tbuf);
	symsiz = getw(tbuf);
	getw(tbuf);
	getw(tbuf);
	if (getw(tbuf) != 0) {
		fprintf(stderr,"No relocation bits\n");
		exit(1);
	}
	if ((datsiz+bsssiz) > 6*8192L) {
		fprintf(stderr,"combined data and bss size too big\n");
		exit(1);
	}
	if (txtsiz > 7*8192L) {
		fprintf(stderr,"text size too big\n");
		exit(1);
	}
	if ((txtsiz+datsiz+symsiz) > 7*8192L-1500) {
		fprintf(stderr,"Warning: ");
		fprintf(stderr,"combined program size too big for boot loader\n");
	}
	putw(0407, obuf);
	putw((short)txtsiz, obuf);
	putw((short)datsiz, obuf);
	putw(bsssiz, obuf);
	putw(symsiz, obuf);
	putw(0, obuf);
	putw(0, obuf);
	putw(1, obuf);
	datrel = -txtsiz;
/*
 *  Copy out data first
 */
	fseek(tbuf, 020+txtsiz, 0);
	fseek(rbuf, 020+txtsiz+txtsiz+datsiz, 0);
	s = datsiz >> 1;
	while (s--) {
		word = getw(tbuf);
		rel = getw(rbuf);
		if (rel&01)
			word -= datrel;
		word += getrel(rel);
		putw(word, obuf);
	}
/*
 * Now to the text.
 */
	fseek(rbuf, 020+txtsiz+datsiz, 0);
	fseek(tbuf, (long) 020, 0);
	s = txtsiz>>1;
	while(s--) {
		rel = getw(rbuf);
		word = getw(tbuf);
		if (rel&01)
			word -= txtrel;
		word += getrel(rel);
		putw(word, obuf);
	}
/*
 * The symbol table.
 */
	fseek(tbuf, 020+txtsiz+txtsiz+datsiz+datsiz, 0);
	s = symsiz;
	while (s) {
		fread(symbuf, 12, 1, tbuf);
		switch(symbuf[4]&07) {
			case 2:
				symbuf[5] += txtrel;
				break;
			case 3:
			case 4:
				symbuf[5] += datrel;
		}
		fwrite(symbuf, 12, 1, obuf);
		if (strncmp(symbuf, "_utsname", 8)==0 && symbuf[4]==043)
			utsseek = symbuf[5];
		s -= 12;
	}
	if (utsseek && utsname[0]) {
		fseek(obuf, (long)020+utsseek, 0);
		fwrite(utsname, 8, 1, obuf);
	}
	chmod(argv[2],0744);
}

getrel(r)
{
	int	off;
	switch (r&016) {

	case 02:	/* ref to text */
		return(txtrel);
	case 04:		/* ref to data */
	case 06:		/* ref to bss */
		return(datrel);
	case 0:
		return(0);
	case 010:
		off = ((r>>4) & 07777) * 12;
		fseek(symfd, 020+txtsiz+txtsiz+datsiz+datsiz+off,0);
		fread(symname, 8, 1, symfd);
		fprintf(stderr,"Undefined Symbol: %s\n",symname);
		return(0);
	default:
		fprintf(stderr,"Bad relocation %o\n", r);
		return(0);
	}
}
