/*
 * This program is used to transfer program files from the unix system
 * to either the EH 4A/BPS4 prom programmer or a DATA I/O prom 
 * programmer or a PLEXUS system which is
 * running a debugging program. The program options are as follows:
 *
 *	-a xxxx		Sets xxxx as the base address for text relocation.
 *			Where xxxx is a hex number. This is also sent to
 *			the PLEXUS monitor if the program is in that mode.
 *
 *	-b xxxx		Sets xxxx as the base address for bss relocation,
 *			where xxxx is a hex number.  This is also sent to
 *			the PLEXUS monitor if the program is in that mode.
 *
 *	-i		Initializes the EH-4A PROM programmer, and programs
 *			the PROM as well as doing the dnld.
 *
 *	-c		Puts a checksum (so that the words will sum to 0)
 *			at location 0x0ffe.  Used for making proms so that
 *			they can be checked for integrity.
 *
 *	-t info		If the output file is a tty then info is used to
 *			set up the terminals options. This is done by first
 *			opening the terminal and then issuing an stty command
 *			to it with info as the parameters.
 *
 *	-o outf		Sets the output file name to outf.
 *
 *	-f inf		Sets the input file name to inf.
 *
 *	-k promsize	Determines the size of the proms being programmed.
 *			(Promsize is multiplied by 1024 to get the actual
 *			promsize.)  (The n'th 2Kb is sent where 'n' is the
 *			number of '+'s-1  preceeding the promsize.)
 *
 *	-l		Causes the low byte of each instruction in inf to
 *			be output to outf. Used only for prom programming.
 *
 *	-h		Causes the high byte of each instruction in inf to
 *			be output to outf. Used only for prom programming.
 *
 *	-p		Sets the program to output data in the format used
 *			by the EH prom programmer.
 *
 *	-z		Sets the program to output data in the format used
 *			by the PLEXUS monitor.
 *
 *	-s xxxx		Sets the segment number sent to the PLEXUS monitor.
 *			xxxx is a hex number.
 *
 *	-u		Used for downloading UNIX thru the boot program,
 *	
 *	-v		Used for the funny fast 2732's from INTEL.
 *
 *	-y xxxx		Sets the communications address for loading the SIOC.
 *
 *	-d		Used for downloading the SIOC.
 *
 *	-B		Used for 4B/BPS4 PROM programmer
 *
 *	-D		Used for the DATA I/O 29A programmer
 *
 *	-F xxyy		Used only for the DATA I/O programmer and must
 *			be present if the -D switch is. xx is the family
 *			and yy is the pinout code (e.g. 1924 for 2732DC).
 *
 *	-L		Object file header contains LONG's as in
 *			68000 type object files
 *
 *

 *
 * The default options are:
 *
 *	-a 0000
 *	-b 0000
 *	-t 1200
 *	-o /dev/promio
 *	-f a.out
 *	-l
 *	-p
 *	-s 0000
 *	-y f800
 */
#include <stdio.h>
#include <a.out.h>
#include <errno.h>

#define TRUE	1
#define FALSE	0
#define MEMSIZE 0X5000


struct exec header;
int arg[3];
unsigned checksum;
char *srcfile;
char *promio;
char str[80];
char *setupptr;
unsigned textstart;
unsigned bssstart;
unsigned chksumflg;
unsigned eh4ainit;
unsigned segnum;
unsigned fsize;
unsigned commadx;		/* SIOC communications area address */
unsigned mem [MEMSIZE];
int hibyte;
FILE *outf;
FILE *inbuf;
int promload;
int dnldunix;
int siocload;
int hinibval;
int lonibval;
int promsize;
int prombase;
int vflag;
int Bflag;
int Dflag;
int Lflag;
char *Fflag;

main (argc, argv)

int argc;
char **argv;
{
char ch;

	Fflag = NULL;
	hibyte = FALSE;
	checksum = 0;
	eh4ainit = FALSE;
	textstart = 0;
	bssstart = 0;
	segnum = 0;
	prombase = 0;
	promsize = 0x800;
	srcfile = "a.out";
	promio = "/dev/eh4a";
	setupptr = "1200";
	promload = TRUE;
	dnldunix = FALSE;
	siocload = 0;
	Lflag =  FALSE;
	commadx = 0xf800;

	while (*++argv != 0) {
		switch(*++*argv) {
		case 'p':
			promload = TRUE;
			break;
		case 'z':
			promload = FALSE;
			break;
		case 's':
			segnum = hatoi(*++argv);
			break;
		case 'a':
			textstart = hatoi(*++argv);
			break;
		case 'b':
			bssstart = hatoi(*++argv);
			break;
		case 'c':
			chksumflg++;
			break;
		case 'h':
			ch = *++*argv;
			if (ch == 'h')
				hinibval++;
			if (ch == 'l')
				lonibval++;
			hibyte = TRUE;
			break;
		case 'i':
			eh4ainit = TRUE;
			break;
		case 'k':
			if (**++argv == '+') {
				prombase = 0x800;
				while (*++*argv == '+')
					prombase += 0x800;
				}
			promsize = 1024 * hatoi(*argv);
			break;
		case 'l':
			ch = *++*argv;
			if (ch == 'h')
				hinibval++;
			if (ch == 'l')
				lonibval++;
			hibyte = FALSE;
			break;
		case 'f':
			srcfile = *++argv;
			break;
		case 'o':
			promio = *++argv;
			break;
		case 't':
			setupptr = *++argv;
			break;
		case 'u':
			dnldunix++;
			break;
		case 'v':
			vflag++;
			break;
		case 'y':
			commadx = hatoi(*++argv);
			break;
		case 'd':
			promload = FALSE;
			siocload++;
			break;
		case 'B':
			Bflag++;
			break;
		case 'D':
			Dflag++;
			break;
		case 'F':
			Fflag = *++argv;
			break;
		case 'L':
			Lflag++;
			break;
		case 'M':
			Lflag++;
			dnldunix++;
			break;
		default:
			fprintf(stderr,"dnld:	illegal option (%s)\n",--*argv);
			++*argv;
			break;
		}
	}

	if(!siocload) {
		if (NULL == (outf = fopen(promio, "w"))) {
			fprintf(stderr, "dnld:	unable to open output file: %s\n", promio);
			exit(1);
		}
	}
	if(isatty(fileno(outf))) {
		sprintf(str, "stty %s < %s", setupptr, promio);
		system(str);
	}
	if ((fsize = memload()) == FALSE) {
		exit(1);
	}

	if (dnldunix)
		loadunix();
	else if (promload == TRUE)
		loadprom();
	else if ( siocload )
		loadsioc();
	else
		loaddebug();
	exit(0);
}



puthex (val)
unsigned val;
{
	if (hibyte == TRUE)
		val >>= 8;
	if (lonibval)
		val &= 0xf;
	if (hinibval) {
		val >>= 4;
		val &= 0xf;
	}
	checksum += val & 0377;
	putnibval(val>>4);
	putnibval(val);
}


putnibval (val)
unsigned val;
{

	val = val & 017;

		if (val <= 9)
			putc(val + '0', outf);
		else
			putc(val + 'A' - 10, outf);
}



memload() {

int i;
unsigned loc;
unsigned rel;

		if ((inbuf = fopen (srcfile,"r")) == 0) {
			fprintf(stderr,"dnld:	%s : could not open\n",srcfile);
			return (FALSE);
		}

		if (dnldunix) return(1);

		if (Lflag) getw(inbuf);
		header.a_magic = getw(inbuf);
		if (Lflag) getw(inbuf);
		header.a_text = getw(inbuf);
		if (Lflag) getw(inbuf);
		header.a_data = getw(inbuf);
		if (Lflag) getw(inbuf);
		header.a_bss = getw(inbuf);
		if (Lflag) getw(inbuf);
		header.a_syms = getw(inbuf);
		if (Lflag)
		{
			getw(inbuf);
			header.a_flag = getw(inbuf);
			getw(inbuf);getw(inbuf);
			getw(inbuf);
			header.a_entry = getw(inbuf);
		}
		else
		{
		header.a_entry = getw(inbuf);
		getw(inbuf);
		header.a_flag = getw(inbuf);
		}

		if (!Lflag && header.a_flag == 1) {
			fprintf(stderr,"dnld:	no relocation information\n");
			return(FALSE);
		}

		for (i=0; i<MEMSIZE; i++)
			mem[i] = 0;

		for (i=0; i<header.a_text/2; i++)
			mem[i] = getw(inbuf);

		for (i=0; i<header.a_data/2; i++)
			mem[i+header.a_text/2] = getw(inbuf);

		for (i=0; i<(header.a_text + header.a_data)/2; i++) {
			if (!Lflag) {
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
				{ int fd; long loc; struct nlist errsym;

				fd = open(srcfile,0);
				loc = (sizeof header)
					+ 2 * (header.a_text + header.a_data)
					+ (rel>>4) * sizeof errsym;
				lseek(fd,loc,0);
				read(fd,&errsym,sizeof errsym);
				close(fd);
				fprintf(stderr,"dnld:	unresolved external (%.8s)\n",errsym.n_name);
				}
				break;
			}
			}
			checksum += mem[i];
		}
		if (chksumflg) {
			checksum -= mem[prombase+promsize-1];
			mem[prombase+promsize-1] = -checksum;
		}
		checksum = 0;
		return (header.a_text + header.a_data);
}


loadsioc()
{
		int	i, fd;
		extern	errno;

		if ( (fd = open( promio, 1)) != (-1) ) {
			if ( lseek( fd, (long)textstart, 0) != textstart ) {
				fprintf( stderr, "dnld:	cannot seek to load position!\n" );
				return(0);
			}
			if ( write( fd, mem, fsize ) == (-1) ) {
				fprintf( stderr, "dnld:	write to %s failed!\n",
					promio);
				return(0);
			}
			close( fd );
		} else if (errno == ENXIO) {
			exit(1);
		} else fprintf( stderr, "dnld:	cannot open %s!\n", promio );
}

loadprom() {
	int i;

	fprintf(stderr, "\007");
	if (eh4ainit) {
		if (Dflag) {
			if (Fflag == NULL) {
				fprintf(stderr,"F switch required\n");
				exit(1);
				}
			fprintf(stderr, "dnld:	initializing DATA I/O PROM programmer ... ");

			/* fprintf(outf,"50A\r"); fflush(outf); sleep(5); */
			fprintf(stderr,"done\n");
		} else {
			fprintf(stderr, "dnld:	initializing EH-4A PROM programmer ... ");
			fprintf(outf, "\n"); fflush(outf); sleep(2);
			if (promsize == 0x800) {
				fprintf(outf, "D5\n"); fflush(outf); sleep(2);
			}
			else {
				if (vflag) {
					fprintf(outf, "DC\n"); fflush(outf); sleep(2);
				}
				else {
					fprintf(outf, "D7\n"); fflush(outf); sleep(2);
				}
			}
			if (Bflag) {
				fprintf(outf, "FS\n"); fflush(outf); sleep(2);
			}
			else {
				fprintf(outf, "F0S1\n"); fflush(outf); sleep(2);
			}
			fprintf(stderr, "done\n");
		}
	}
	fprintf(stderr, "dnld:	downloading %s to prom programmer ... ",srcfile);
	if (!Dflag) putc('\002', outf);
	if (Dflag) {
		fprintf(outf,"CO\r");fflush(outf);sleep(2);
		fprintf(outf,"PO\r");fflush(outf);sleep(2);
		fprintf(outf,"0000 %x TO RA\r",promsize);fflush(outf);sleep(2);
		fprintf(outf,"0000\r");fflush(outf);sleep(2);
		fprintf(outf,"\002$A0000,"); fflush(outf); sleep(2);
		for (i=prombase;i<prombase+promsize; i++) {
			puthex(mem[i]);
			putc(' ',outf);
			}
		}
	else if (Bflag)
		for (i=prombase; i<prombase+promsize; i++) {
			puthex(mem[i]);
			putc(' ', outf);
		}
	else for (i=prombase; i<prombase+0x800; i++) {
		puthex(mem[i]);
		putc(' ', outf);
	}
	if (!Dflag) putc('\003', outf);
	fflush(outf);
	fprintf(stderr, "done, checksum = %x\n",checksum);
	if (eh4ainit) {
		sleep(5);
		fprintf(stderr, "dnld:	programming PROM ... ");
		if (Dflag) {
			fprintf(outf,"\003                ");fflush(outf);sleep(2);
			/* fprintf(outf,"$S%04x,",checksum);fflush(outf);sleep(2); */
			fprintf(outf,"CO\r");fflush(outf);sleep(2);
			fprintf(outf,"RA\r");fflush(outf);sleep(2);
			fprintf(outf,"0000 %x TO DE\r",promsize);fflush(outf);sleep(2);
			fprintf(outf,"0000\r");fflush(outf);sleep(2);
			fprintf(outf,"%s\r",Fflag);fflush(outf);sleep(2);
			fprintf(outf,"\r");fflush(outf);sleep(2);
			}
		else
		if ((promsize == 0x800) || Bflag) {
			fprintf(outf, "APS\n");	fflush(outf); sleep(2);
		}
		else {
			if ((prombase % 0x1000) == 0) {
				fprintf(outf, "AP\r"); fflush(outf); sleep(2);
				fprintf(outf, "B1\r"); fflush(outf); sleep(2);
			}
			else {
				fprintf(outf, "AP\r"); fflush(outf); sleep(2);
				fprintf(outf, "B2\r"); fflush(outf); sleep(2);
			}
		}
		sleep(105);
		if ((Dflag||Bflag) && (promsize > 0x800))
			sleep(105);
		fprintf(stderr, "should be done\n");
	}
	/* if (Dflag)
		fprintf(outf,"Z\r"); */
	fprintf(stderr, "\007"); sleep(1); fprintf(stderr, "\007");
	exit(0);
}


loaddebug()
{
	int i;

	fprintf(stderr,"\007dnld:	downloading %s to monitor ... ",srcfile);
	fprintf(outf, "?%02x,%04x,%04x,", segnum, textstart, fsize);
	for (i = 0; i < (fsize / 2); i++) {
		hibyte = TRUE;
		puthex(mem[i]);
		hibyte = FALSE;
		puthex(mem[i]);
	}
	fprintf(outf, ",%04x", checksum);
	fprintf(stderr, "\007done, checksum = %x\007\n",checksum);
}


loadunix() {
	unsigned val;
	int i;
	int count;

	if (Lflag) 
		fprintf(stderr,"\007dnld:	downloading %s to m68000 ... ",srcfile);
	else
		fprintf(stderr,"\007dnld:	downloading %s to boot program ... ",srcfile);
	fprintf(outf,"?");
	for (i=0; i<16; i++) {
		puthex(getc(inbuf));
		if (Lflag)
			puthex(getc(inbuf));
	}

	fflush(outf);
	sleep(5);

	count = 32;
	while( (val = getc(inbuf)) != EOF) {
		puthex(val);
		if (++count == 512) {
			fflush(outf);
			for(i=0; i<2000; i++);
			count = 0;
		}
	}
	sleep(1);
	fflush(outf);
	fprintf(outf,",");
	fflush(outf);
	sleep(1);
	fprintf(outf,"%04x",checksum);
	fprintf(stderr,"\007done, checksum = %x\007\n",checksum);
	if (Lflag)
		fprintf(outf,".");
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
