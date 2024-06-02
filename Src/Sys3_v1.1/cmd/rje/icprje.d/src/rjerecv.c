/*
 *	RJERECV - reads, demultiplexes, and expands
 *			data received from the VPM.
 *
 *		rjerecv( fsmin , max )
 */

#include <rje.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ustat.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

/*
 * received record types and RJEDISP commands
 */
#define PRINTER		0
#define PUNCH		1
#define CONSOLE		2

/*
 * protocol info for RJEXMIT
 */
#define XMITGRANT	'G'	/* IBM has granted our request */
#define IBMREQ		'R'	/* IBM request */
#define XMITWAIT	'W'	/* stop card reader */
#define XMITSTART	'S'	/* start card reader */

/*
 * output dev structure, one per printer, punch and console
 */
struct dev {
	int fd;			/* open file descriptor */
	char file[8];		/* current file: p[ru]XXXXX */
	off_t size;		/* current file size (bytes) */
	long numrec;		/* current file size (records) */
	short flag;		/* state */
};

/* flag bits */
#define OPEN	01		/* dev has an open file */
#define TRUNC	02		/* too much output received on dev */
#define XEOF	04		/* EOF received on dev */
#define ABORT	010		/* IBM aborted transmission on dev */

/*
 * macro to convert an RCB into a dev index (0-6)
 */
#define devno(x) (((x>>4)&07)-1)

/*
 * commmon
 */
daddr_t fsmin;			/* minimum file system space on rjedev */
off_t max;			/* maximum file size (bytes) */
dev_t rjedev;			/* device RJE resides on */
char who[10];			/* prefix of name, e.g. "rje" */
struct dev cons;		/* console device */
struct dev prnt[MAXDEVS];	/* printers */
struct dev pnch[MAXDEVS];	/* punches */

/*
 * misc
 */
int sigterm(),sigpipe();
extern int errno;

main(argc,argv)
	char **argv;
{
	struct stat sb;
	long atol();

	if(argc != 3) {
		errlog("arg count\n");
		goto badstart;
	}
	name(argv[0],"recv",who);
	fsmin = atol(argv[1]);
	max = atol(argv[2]) << 9;
	if((cons.fd=open("resp",O_WRONLY)) < 0) {
		errlog("can't open resp file\n");
		goto badstart;
	}
	lseek(cons.fd,0L,2);
	cons.flag |= OPEN;
	if(chdir("rpool") < 0) {
		errlog("can't chdir to rpool\n");
		goto badstart;
	}
	stat(".",&sb);
	rjedev = sb.st_dev;
	signal(SIGTERM,sigterm);
	signal(SIGPIPE,sigpipe);

	/*
	 * Process received blocks of the form:
	 *	BCB, FCS, data ...
	 */
	while(1) {
		blockctl();
		fcnctl();
		blockdata();
	}

badstart:
	stoprje();
	rjedead("RJE error\n",".");
}

/*
 * Process the data of a block:
 *	REC1, REC2, REC3, ... end of block record
 */
blockdata()
{
	register unsigned int rcb;
	register unsigned int n;

	/*
	 * The implemented record types are:
	 *	0x00 - end of block indicator
	 *	0x90 - IBM request to initiate transmission
	 * 	0xa0 - IBM grant to our request to initiate transmission
	 *	0xN4 - print record for dev N (1-7)
	 *	0xN5 - punch record for dev N
	 *	0x91 - console message
	 */

	while((rcb = getvpm()) != 0) {

		rcb = rcb & 0xff;

		switch(rcb & 0xf) {
		case 0x0:
			if(rcb == 0x90) {
				if(freespace()) {
					putxmit(IBMREQ);
					putxmit(getvpm());
					putxmit(-1);
				} else {
					errlog("file space is low\n");
					getvpm();
				}
				getvpm();
			} else if(rcb == 0xa0) {
				n = devno(getvpm()) & 0xf;
				if(n < MAXDEVS) {
					putxmit(XMITGRANT);
					putxmit(n);
					putxmit(-1);
				}
				getvpm();
			} else {
				goto badrcb;
			}
			break;

		case 0x1:
			datarecord(&cons,CONSOLE);
			break;
		case 0x4:
			n = devno(rcb) & 07;

			if(n >= MAXDEVS) {
				goto badrcb;
			}
			datarecord(&prnt[n],PRINTER);

			break;
		case 0x5:
			n = devno(rcb) & 07;

			if(n >= MAXDEVS)
				goto badrcb;

			datarecord(&pnch[n],PUNCH);
			break;

		default:

			goto badrcb;
		}
	}
	return;

badrcb:
	errlog("unimplemented RCB function\n");

	while (n=getvpm() != 0) {
	}
	return;
/*
	stoprje();
	reboot("Line errors\n",who,"../");
*/
}

errlog1(s,a)
char *s;
{
	char buf[100];
	sprintf(buf,s,a);
	write(ERRFD,buf,strlen(buf));
}

/*
 * Process the first byte of a block.  This is NOT
 * the protocol BCB but a report passed back
 * by the VPM having the following meaning:
 */
#define ALL_OK	0	/* everything seems fine */
#define INITFLT	1	/* IBM not ACKing out ENQ */
#define ERRORS	2	/* excessive line errors */

blockctl()
{
	register int b;

	if((b=getvpm()) != ALL_OK) {
		errlog("read VPM error\n");


		if(b == INITFLT)
			stoprje();
			reboot("IBM not responding\n",who,"../");
		if(b == ERRORS)
			stoprje();
			reboot("Line errors\n",who,"../");
		reboot("RJE error\n",who,"../");
	}
}

/*
 * process the function control sequence (FCS).
 */
fcnctl()
{
	register int fcs;
	register int n;
	register int mask;
	static int devstate = 0xfe;

	/*
	 * check IBM multileaving document for FCS meaning!
	 */
	fcs = (getvpm() & 0xf) << 4;
	fcs |= (getvpm() & 0xf);
	if((fcs & 0xfe) != 0xfe) {
		mask = 0x80;
		for(n=0; n < MAXDEVS; n++) {
			if(((mask & fcs) & 0xff) == 0) {
				devstate &= ~mask;
				putxmit(XMITWAIT);
				putxmit(n);
			} else {
				if(((devstate & mask) & 0xff) == 0) {
					devstate |= mask;
					putxmit(XMITSTART);
					putxmit(n);
				}
			}
			mask >>= 1;
		}
		putxmit(-1);
	}
}

/*
 * Expand and save a record of data on the dev passed.
 * type is PRINTER, PUNCH or CONSOLE.
 */
datarecord(dev,type)
	register struct dev *dev;
	register int type;
{
	struct dsplog dsplog;
	char buf[512];
	int n;

if (type == CONSOLE)
	goto consolego;

	if((dev->flag & OPEN) == 0) {

		makename(dev->file,type==PRINTER? "prXXXXX":"pnXXXXX");
		while((dev->fd=creat(dev->file,0644)) < 0)
			errlog("can't create tmp file in rpool\n");
		dev->flag |= OPEN;
		dev->size = 0;
		dev->numrec = 0;
	}
consolego:
	if(type == CONSOLE) {
		if(dev->size > RESPMAX) {
			stoprje();
			reboot("Rebooting\n",who,"../");
		}
	} else {
		if(dev->size > max)
			dev->flag |= TRUNC;
	}

	n = (unsigned int)getrec(buf,dev,type);

	if(dev->flag & ABORT) {

		errlog("abort print file\n");

		close(dev->fd);
		unlink(dev->file);
		dev->flag = 0;
		return;
	}

	if((dev->flag & (TRUNC | XEOF)) == 0) {

		write(dev->fd,buf,n);
		dev->size += n;
		dev->numrec++;
	}

	if(type == CONSOLE) {
		dev->flag = 0;
		return;
	}
	if(dev->flag & XEOF) {

		close(dev->fd);
		dsplog.d_type = type;
		strcpy(dsplog.d_un.dspr.d_file,dev->file);
		dsplog.d_un.dspr.d_cnt = dev->numrec;
		dsplog.d_un.dspr.d_trunc = (dev->flag&TRUNC)? 1:0;
		dev->flag = 0;
		write(DSPWR,&dsplog,sizeof(struct dsplog));
	}
}

/*
 * Gather a print record into buf for dev.  Return
 * the number of bytes read.  Record format:
 *	SRCB SCB1 data SCB2 data ... end_record_scb
 * All SCBS are implemented:
 *	0000 0000 - end of record (EOF)
 *	0100 0000 - abort transmission
 *	100n nnnn - insert nnnnn blanks
 *	101n nnnn - insert nnnnn of the next byte
 *	11nn nnnn - insert the next nnnnnn bytes
 */
getrec(buf,dev,type)
	struct dev *dev;
	char *buf;
{
	unsigned int srcb;
	register char *bp;
	register unsigned int n;
	register unsigned int b;

	srcb = getvpm();
	b=getvpm();
	if((b & 0xff) == 0) {
		dev->flag |= XEOF;
		return(0);
	}
	if(type == PUNCH)
		bp = &buf[1];
	else
		bp = buf;
	for(; b != 0; b=(unsigned int)getvpm()) {
		if((b & 0xc0) == 0xc0) {
			n = b & 0x3f;
			while(n--)
				*bp++ = getvpm();
		} else if((b & 0xc0) == 0x80) {
			n = b & 0x1f;
			if(b & 0x20)
				b = getvpm();
			else if(type==PUNCH)
				b = 0x40;
			else
				b = ' ';
			while(n--)
				*bp++ = b;
		} else {
			while( getvpm() != 0) {
			}
			dev->flag |= XEOF;
			return(0);
/*
			dev->flag |= ABORT;
			return(0);
*/
		}
	}
	if(type == PUNCH) {
		n = (int)(bp - buf);
		buf[0] = n-1;
		return(n);
	} else if(type == CONSOLE) {
		*bp++ = '\n';
		return((unsigned int)(bp - buf));
	} else {
		while(*--bp == ' ')
			;
		bp++;
		if((srcb & 0xf0) == 0x80) {
			n = srcb & 0x3;
			while(n--)
				*bp++ = '\n';
		} else if(srcb == 0x91) {
			*bp++ = ' ';
			*bp++ = '\n';
		} else
			*bp++ = '\n';
		return((unsigned int)(bp-buf));
	}
}

/*
 * Check if there is enough free space on rjedev
 * Return 0 if not, else return 1
 */
freespace()
{
	struct ustat usb;

	ustat(rjedev,&usb);
	if(usb.f_tfree <= fsmin) {
		return(0);
	}
	return(1);
}

/*
 * Generate a file name.  Temp must be of the form:
 *	prefixXXXXX
 * The name is put into buf with XXXXX filled
 * in by a number to make buf unique in the current dir.
 */
makename(buf,temp)
char *buf,*temp;
{
	static num;
	register char *s1,*s2;
	register int i;

	do {
		s1 = buf;
		s2 = temp;
		i = num;
		while(*s1++ = *s2++)
			;
		s1--;
		while(*--s1 == 'X') {
			*s1 = (i%10) + '0';
			i /= 10;
		}
		if(++num < 0)
			num = 0;
	} while(access(buf,0) == 0);
}

/*
 * Log RJE errors to ERRFD
 */
errlog(msg)
char *msg;
{
	write(ERRFD,msg,strlen(msg));
}

/*
 * Clean up rpool directory
 */
cleanup()
{
	register int i;
	register struct dev *dev;

	signal(SIGTERM,SIG_IGN);
	for(i=0; i < MAXDEVS; i++) {
		dev = &prnt[i];
		if(dev->flag & OPEN) {
			close(dev->fd);
			unlink(dev->file);
		}
		dev = &pnch[i];
		if(dev->flag & OPEN) {
			close(dev->fd);
			unlink(dev->file);
		}
	}
}

/*
 * Halt RJE and clean up.
 */
stoprje()
{
	signal(SIGTERM,SIG_IGN);
	kill(0,SIGTERM);
	cleanup();
}

/*
 * Cleanup and exit on SIGTERM
 */
sigterm()
{
	cleanup();
	exit(0);
}

/*
 * Catch pipe errors
 */
sigpipe()
{
	errlog("pipe to xmit/disp broken\n");
	stoprje();
	reboot("RJE error\n",who,"../");
}

/*
 * Return the next byte from the VPM.
 */
getvpm()
{
	static char buf[512];
	static char *bp;
	static   int n = 0;

	while(n == 0) {
		n = read(DEVFD,buf,512);
		if(n <= 0) {
			n = 0;
			if(errno == EINTR) {
				/* signal hit */
				continue;
			} else {
				errlog("vpm read error\n");
/*
				stoprje();
				reboot("VPM error\n",who,"../");
*/
			}
		}
		bp = buf;
	}
	--n;
	return(*bp++ & 0377);
}

/*
 * Buffer byte to be sent to xmit.  If
 * byte < 0, flush the buffer
 */
putxmit(byte)
int byte;
{
	static char buf[20];
	static char *bp = buf;
	static char n = 0;

	if(byte < 0) {
		write(XMTWR,buf,n);
		n = 0;
		bp = buf;
	} else {
		*bp++ = byte;
		n++;
	}
}
