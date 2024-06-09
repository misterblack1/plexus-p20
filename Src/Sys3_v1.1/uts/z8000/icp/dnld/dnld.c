/*
 * This program is used to transfer program files from the main processor
 * to a down loadable controller.
 *
 *	-a xxxx		Sets xxxx as the base address for text relocation.
 *			Where xxxx is a hex number. 
 *
 *	-b xxxx		Sets xxxx as the base address for bss relocation,
 *			where xxxx is a hex number.
 *
 *	-o outf		Sets the output file name to outf.
 *
 *	-f inf		Sets the input file name to inf.
 *
 */
#include "errno.h"
#include "stdio.h"
#include "a.out.h"


#define TRUE	1
#define FALSE	0
#define MEMSIZE 0X5000


struct exec header;
int arg[3];
unsigned checksum;
char *srcfile;
char *destfile;
char str[80];
unsigned textstart;
unsigned bssstart;
unsigned fsize;
unsigned mem [MEMSIZE];
FILE *outf;
FILE *inbuf;

main (argc, argv)

int argc;
char **argv;
{
	checksum = 0;
	textstart = 0;
	bssstart = 0;
	srcfile = "";
	destfile = "";

	while (*++argv != 0) {
		if (**argv != '-')
			fprintf(stderr,"dnld: invalid request\n");
		switch(*++*argv) {
		case 'a':
			textstart = hatoi(*++argv);
			break;
		case 'b':
			bssstart = hatoi(*++argv);
			break;
		case 'f':
			srcfile = *++argv;
			break;
		case 'o':
			destfile = *++argv;
			break;
		}
	}

	if ((fsize = memload()) == FALSE) {
		exit(1);
	}

	loadsioc();
}


memload() {

int i;
unsigned loc;
unsigned rel;

	if ((inbuf = fopen (srcfile,"r")) == 0) {
		fprintf(stderr,"dnld: could not open %s\n",srcfile);
		return (FALSE);
	}

	header.a_magic = getw(inbuf);
	header.a_text = getw(inbuf);
	header.a_data = getw(inbuf);
	header.a_bss = getw(inbuf);
	header.a_syms = getw(inbuf);
	header.a_entry = getw(inbuf);
	header.a_stamp = getw(inbuf);
	header.a_flag = getw(inbuf);

	if (header.a_flag == 1) {
		fprintf(stderr,"dnld: no relocation information\n");
		return(FALSE);
	}

	for (i=0; i<MEMSIZE; i++)
		mem[i] = 0;

	for (i=0; i<header.a_text/2; i++)
		mem[i] = getw(inbuf);

	for (i=0; i<header.a_data/2; i++)
		mem[i+header.a_text/2] = getw(inbuf);

	for (i=0; i<(header.a_text + header.a_data)/2; i++) {
		rel = getw(inbuf);
		switch (rel) {
		case 2:
		case 4:
			mem[i] += textstart;
			break;
		case 6:
			if (bssstart)
				mem[i] += bssstart - header.a_text - header.a_data;
			else
				mem[i] += textstart;
			break;
		case 0:
			break;
		default:
			fprintf(stderr,"dnld: unresolved external (%x)\n",rel);
			break;
		}
		checksum += mem[i];
	}
	return (header.a_text + header.a_data);
}


loadsioc()
{
	int	i, fd;
	extern errno;

	if ( (fd = open( destfile, 1)) != (-1) ) {
		if ( lseek( fd, (long)textstart, 0) != (long)textstart ) {
			fprintf( stderr, "dnld: cannot seek to load position!\n" );
			return(0);
		}
		if ( write( fd, mem, fsize ) == (-1) ) {
			printf( stderr, "dnld: write to SIOC failed!\n" );
			return(0);
		}
		close( fd );
	/* does ICP exit? if not, then quit */
	} else if(errno == ENXIO) {
		exit(1);
	}
	else
		fprintf(stderr, "dnld: cannot open %s\n", destfile);
}


hatoi(p)
register char *p;
{
	register int n;
	register int f;

	n = 0;
	f = 0;
	for(;;p++) {
		switch(*p) {
		case ' ':
		case '\t':
			continue;
		case '-':
			f++;
		case '+':
			p++;
		}
		break;
	}
	for(;;) {
		switch (*p) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			n = n*16 + *p++ - '0';
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			n = n*16 + *p++ - 'a' + 10;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			n = n*16 + *p++ - 'A' + 10;
			break;
		default:
			return(f? -n: n);
		}
	}
}
