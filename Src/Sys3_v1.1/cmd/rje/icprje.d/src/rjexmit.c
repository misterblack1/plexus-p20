/*
*	rjexmit - Transmits files via the kmc
*
*		rjexmit( home , readers )
*/

#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <rje.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define WAIT	2
#define TRUE	1
#define FALSE	0
#define QFULL	0
#define END	0
#define BSIZ	512
#define RECSZ	63	/* Maximum record size */
#define QSIZE	6	/* Size of internal Queue */
#define MSG	"co"	/* File prefix for Messages */
#define SEQ	"sq"	/* File prefix for sequential xmit files */
#define START	3	/* Identifies file started to disp */
#define SENT	4	/* Identifies file sent to disp */

#define SP	0x40			/* IBM space character */
#define EOR	0x00			/* End of Record */
#define REQ	0x90			/* Request byte (rcb) */
#define PERM	0xa0			/* Permission byte (rcb) */
#define MSGRCB	0x92			/* Message RCB */
#define ISRCB	0x80			/* Input record SRCB */
#define ABORT	0x40			/* Abort input SCB */
#define RDR(x)	(char)((x<<4) + 0x93)	/* Reader ID byte */
#define PRT(x)	(char)((x<<4) + 0x94)	/* Printer ID byte */
#define PUN(x)	(char)((x<<4) + 0x95)	/* Punch ID byte */
#define CCNT(x)	(char)(x + 0xc0)	/* Character count SCB */

		/* Queue increment macro */
#define BUMP(x) (x = (x == (QSIZE-1)) ? 0 : ++x);

extern int errno;

struct queue {		/* Internal Queue format */
	struct dsplog d[QSIZE];
	long q_cnt;
	int q_top;
	int q_bot;
	int q_open;
	int q_size;
};

struct queue jque[MAXDEVS];	/* Queues for maximum readers */
struct queue mque;		/* Message Queue */
struct joblog lbuf;		/* Structure of Joblog file */

char buf[BSIZ], who[12];
char stopf[48], home[32];
int rdrs;

struct stat stb;
int pipesig(), termsig();

main(ac,av)
int ac;
char **av;
{

	if(ac != 3)
		error("Arg count\n",1);
	name(*av,"xmit",who);
	strcpy(home,*(++av));
	strcpy(stopf,*(av));
	strcat(stopf,"/stop");
	rdrs = atoi(*(++av));
	signal(SIGPIPE,pipesig);
	signal(SIGTERM,termsig);

/*
*	Change to the squeue (queuer pool) directory.
*	Set console open flag to TRUE.
*/

	if(chdir("squeue") != 0)
		error("Can't chdir to squeue\n",1);
	mque.q_open = TRUE;

	sleep(20);

/*
*	Loop forever queuing files and transmitting.
*	If none of either, sleep.
*/

	for(;;) {
		if(getq() | xmit())
			continue;
		else
			sleep(5);
	}
}

/*
*	error handles xmit errors
*/

error(str,fatal)
char *str;
int fatal;
{
	write(ERRFD,str,strlen(str));
	if(fatal) {
		stoprje();
		rjedead(str,home);
	}
}

/*
*	getq sets up the internal queue from the joblog.
*	while there are entries in the joblog:
*	- get a reader
*	- initialize queue info
*	- change count field to reader number
*	- set type for disp
*	- bump queue top, count
*	- open reader if not open
*	if no entries, return FALSE
*/

getq()
{
	long lseek();
	struct queue *getrdr();
	register struct queue *qptr;
	int rdr;

	while(read(JBLOG,&lbuf,sizeof(struct joblog))) {
		if((qptr = getrdr(&rdr)) == QFULL) {
			lseek(JBLOG, (long)-sizeof(struct joblog), 1);
			return(TRUE);
		}
		qptr->q_size += lbuf.j_cnt;
		qptr->d[qptr->q_top].d_type = START;
		qptr->d[qptr->q_top].d_un.x.d_rdr = rdr;
		strcpy(qptr->d[qptr->q_top].d_un.x.d_file,lbuf.j_file);
		qptr->d[qptr->q_top].d_un.x.d_uid = lbuf.j_uid;
		qptr->d[qptr->q_top].d_un.x.d_lvl = lbuf.j_lvl;
		qptr->d[qptr->q_top].d_un.x.d_cnt = lbuf.j_cnt;
		BUMP(qptr->q_top);
		qptr->q_cnt++;
		return(TRUE);
	}
	return(FALSE);
}

/*
*	getrdr returns the message queue if the file name
*	begins with MSG. It returns the highest job queue
*	if the file name begins with SEQ (sequential job).
*	If its a normal file getrdr returns the job que
*	with the least number of cards on it, or QFULL is
*	the queue is full. Queues are logical readers.
*/

struct queue *
getrdr(readr)
int *readr;
{
	register struct queue *qptr, *tptr, *qend;

	if(strncmp(lbuf.j_file,MSG,2) == 0) {
		qptr = &mque;
		*readr = rdrs;
		if(qptr->q_cnt < QSIZE)
			return(qptr);
		else
			return(QFULL);
	}
	if(strncmp(lbuf.j_file,SEQ,2) == 0) {
		qptr = &jque[rdrs-1];
		*readr = rdrs - 1;
		if(qptr->q_cnt < QSIZE)
			return(qptr);
		else
			return(QFULL);
	}
	qend = &jque[rdrs];
	qptr = &jque[0];
	for(tptr = &jque[1]; tptr < qend; tptr++) {
		if(tptr->q_size < qptr->q_size)
			qptr = tptr;
	}
	if(qptr->q_cnt < QSIZE) {
		*readr = qptr - &jque[0];
		return(qptr);
	}
	return(QFULL);
}

/*
*	xmit transmits a block from each open job, and
*	checks the recv pipe for any info.
*/

xmit()
{
	static int job[MAXDEVS];
	static int msg;
	register int i, exitval = 0;

	for(i = 0; i < rdrs; i++) {
		job[i] = dojob(job[i], &jque[i], i);
		exitval += jque[i].q_cnt;
	}
	msg = dojob(msg, &mque, i);
	ckrecv();
	exitval += mque.q_cnt;
	return(exitval);
}

/*
*	dojob uses sndblk to send a block from the specified
*	file descriptor (fd). If there is no file descriptor
*	FALSE is returned. If the job has been sent, the file
*	descriptor is closed, the queue is updated and FALSE returned.
*/

dojob(fd,q,rdr)
register int fd;
register struct queue *q;
register int rdr;
{
	if(fd == 0) {
		if(q->q_cnt > 0) {
			if((fd = open(q->d[q->q_bot].d_un.x.d_file,O_RDONLY)) < 0) {
				error("Can't open file\n",0);
				q->q_size -= q->d[q->q_bot].d_un.x.d_cnt;
				BUMP(q->q_bot);
				--(q->q_cnt);
				return(0);
			}
			if(rdr < rdrs) {
				openinp(rdr);
				write(DSPWR,&(q->d[q->q_bot]),sizeof(struct dsplog));
			}
		} else
			return(0);
	}
	if(sndblk(fd,rdr,q) == END) {
		close(fd);
		unlink(q->d[q->q_bot].d_un.x.d_file);
		if(rdr < rdrs) {
			q->d[q->q_bot].d_type = SENT;
			write(DSPWR,&(q->d[q->q_bot]),sizeof(struct dsplog));
			closeinp(rdr);
		}
		q->q_size -= q->d[q->q_bot].d_un.x.d_cnt;
		BUMP(q->q_bot);
		--(q->q_cnt);
		return(0);
	}
	return(fd);
}

/*
*	sndblk sends a block from the specified file descriptor (fd).
*/

sndblk(fd,rdr,q)
int fd, rdr;
struct queue *q;
{
	register char *ptr;
	char line[80], ch, rcb, *mkrec();
	register int cnt, msgflg;
	int exitval;

	ptr = buf;
	exitval = TRUE;
	if(jque[rdr].q_open & WAIT)
		return(exitval);
	if(rdr < rdrs) {
		rcb = RDR(rdr);
		msgflg = FALSE;
	} else	{
		rcb = MSGRCB;
		msgflg = TRUE;
	}
	for(;;) {
		if(read(fd,&ch,1) != 1) {
			*ptr++ = EOR;
			exitval = END;
			break;
		}
		if(((cnt = (int)ch) < 0) || (cnt > 80)) {
			error("Illegal byte count on file\n",0);
			return(END);
		}
		if(((ptr - buf) + (cnt + 15)) > BSIZ) {
			*ptr++ = EOR;
			lseek(fd,(long)-1,1);
			break;
		}
		if(read(fd,line,cnt) != cnt) {
			error("File corrupted\n",0);
			return(END);
		}
		ptr = mkrec(rcb,ISRCB,cnt,line,ptr);
/*
 *	The following is done because JES3 throws away the rest
 *	of a block after seeing a console message.
 */
		if(msgflg) {
			*ptr++ = EOR;
			kmcwrite(buf,(ptr - buf));
			ptr = buf;
		}
	}
	if(!msgflg)
		kmcwrite(buf,(ptr - buf));
	return(exitval);
}

/*
*	openinp sends the reader open requests to IBM
*	and sets the reader open flag to WAIT.
*/

openinp(rdr)
register int rdr;
{
	register char *ptr;

	ptr = buf;
	*ptr++ = REQ;
	*ptr++ = RDR(rdr);
	*ptr++ = EOR;
	*ptr++ = EOR;
	kmcwrite(buf,4);
	jque[rdr].q_open |= WAIT;
}

/*
*	closeinp closes the reader specified.
*/

closeinp(rdr)
register int rdr;
{
	register char *ptr;

	ptr = buf;
	*ptr++ = RDR(rdr);
	*ptr++ = ISRCB;
	*ptr++ = EOR;
	*ptr++ = EOR;
	kmcwrite(buf,4);
	jque[rdr].q_open = FALSE;
}

/*
*	mkrec makes a record according to the protocol:
*	RCB | SRCB | SCB | DATA | SCB [ . . ]
*/

char *
mkrec(rcb,srcb,cnt,from,to)
register char *from, *to;
char rcb, srcb;
int cnt;
{
	register int i;

	*to++ = rcb;
	*to++ = srcb;
	if(cnt > RECSZ) {
		*to++ = CCNT(RECSZ);
		for(i = 0; i < RECSZ; i++)
			*to++ = *from++;
		cnt -= RECSZ;
	}
	if(!cnt) {
		*to++ = CCNT(1);
		*to++ = SP;
	} else {
		*to++ = CCNT(cnt);
		for(i = 0; i < cnt; i++)
			*to++ = *from++;
	}
	*to++ = EOR;
	return(to);
}

/*
*	ckrecv checks the recv pipe for any info.
*	Pipe info syntax is an Id byte followed by
*	1 byte of data.
*	G => Grant
*	R => Request
*	W => Wait a bit
*/

ckrecv()
{
	register char *inp;
	char in[64], out[4];
	register int cnt;

	inp = in;
	if(fstat(XMTRD,&stb) < 0) {
		if(stat(stopf,&stb) < 0) {
			error("xmit/recv pipe bad\n",0);
			stoprje();
			reboot("RJE error\n",who,home);
		}
		exit(1);
	}
	if(stb.st_size == 0)
		return;
	cnt = read(XMTRD,in,64);
	for(; cnt > 0; cnt--) {
		switch(*inp++) {
		case 'G':	/* Reader number follows 'G' */
			jque[(int)(*inp++)].q_open = TRUE;
			break;
		case 'R':	/* RCB byte follows 'R' */
			out[0] = PERM;
			out[1] = *inp++;
			out[2] = EOR;
			out[3] = EOR;
			kmcwrite(out,4);
			break;
		case 'W':	/* Wait a bit on input */
			jque[(int)(*inp++)].q_open |= WAIT;
			break;
		case 'S':	/* Restart input */
			jque[(int)(*inp++)].q_open &= (~WAIT);
			break;
		default:
			error("XMIT: Illegal pipe info\n",0);
			++inp;
		}
		cnt--;
	}
}

/*
*	kmcwrite writes "size" bytes from "from" to
*	the kmc device file descriptor.
*	It retries if errno indicates an interrupt.
*/

kmcwrite(from,size)
char *from;
int size;
{
	for(;;) {
		if(write(DEVFD,from,size) == -1) {
			if(errno == EINTR)
				exit(1);
			else	{
				error("Can't write device\n",0);
				stoprje();
				reboot("Can't write device\n",who,home);
				}
		}
		break;
	}
}

/*
*	stoprje signals the process group with SIGTERM
*/

stoprje()
{
	signal(SIGTERM,SIG_IGN);
	kill(0,SIGTERM);
	signal(SIGTERM,termsig);
}

/*
*	pipesig handles SIGPIPE when a write to "rjedisp"
*	fails.
*/

pipesig()
{
	signal(SIGPIPE,pipesig);
	if(stat(stopf,&stb) < 0) {
		error("xmit/disp pipe bad\n",0);
		stoprje();
		reboot("RJE error\n",who,home);
	}
	exit(1);
}

/*
*	termsig handle SIGTERM
*/

termsig()
{
	alarm(0);
	signal(SIGTERM,termsig);
	close(DEVFD);
	exit(0);
}
