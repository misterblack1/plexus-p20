#include "saio.h"
#include "sys/plexus.h"
#include "sys/imsc.h"
#include "ctype.h"

#define reterr ((WUB->cab_etype)<<8 | (WUB->cab_ecode))

#define TAPECAB 0xfe18
#define IMSCWU 0x1f

#define MAXUNIT 3L
#define MINUNIT 0L
#define BLKSPERTAPE 38000L

struct s_iblk {
	unsigned cylinders;
	char remheads;
	char fixheads;
	char lsb_sectsize;
	char sectpertrack;
	char altcylinders;
	char msb_sectsize;
	} pd_init[];

long atol();
long getnum();
long nblocks;
long block;
int tapenum;
unsigned char unit;
char func;

imain(drive,mode)
{
  unit = drive;
  init();
  getparams(drive,mode);
  while (nblocks) 
    movetape();
  switch (mode) {
  case 'b':
	printf("Disk reads complete!\n");
	printf("Backup to tape complete successfully\n");
	break;
  case 'r':
	printf("Restor completed successfully!\n");
	break;
  default:
	printf("Illegal mode %c\n",mode);
	exit(1);
  }
  exit(0);
}

init()
{

  tapenum=1;
  open("pd(0,0)",2);
}

getparams(drive,mode)
{
  long blkcnt;
  char buf[40];

  func = toupper(mode);
  do {
  	printf("Tape unit? [0]: ");
	gets(buf);
  } while (buf[0] != '0' && atol(buf) != 0); /* must be 0; atol returns */
					     /*	0 on non-numeric strings */
  blkcnt = (long)pd_init[unit].fixheads * (long)pd_init[unit].sectpertrack
	* (long)pd_init[unit].cylinders;
  block=getnum("Starting disk block number for backup/restore",0L,blkcnt);
  nblocks=getnum("Number of blocks in backup/restore",0L,blkcnt-block);
}

/*
 *  getnum prompts for a response with optional limits
 *  dislayed and loops until a valid response has been
 *  entered. to turn off limits call with min and max
 *  set to -1
 */
long
getnum(prompt,nmin,nmax)
char *prompt;
long nmin,nmax;
{
  char buf[80];
  long i;

  do {
    printf("%s?",prompt);
    if (~(nmin&nmax))
      printf(" [%ld-%ld]",nmin,nmax);
    printf(": ");
    gets(buf);
    i=atol(buf);
    }
  while ( (~(nmin&nmax)) && ((i<nmin) || (i>nmax)) );
  return(i);
}

movetape()
{
  long tblocks;
  char buf[40];

  if (tapenum > 1) {
    printf("Insert Tape %d, blocks %ld to %ld\nPress return when ready",
      tapenum,block,block+min(BLKSPERTAPE,nblocks)-1L);
    gets(buf);
    }
  rewind();
/*  seek_disk(unit,block); */
  tapestrat(func,block,tblocks=min(nblocks,BLKSPERTAPE));
  block += tblocks;
  nblocks -= tblocks;
  rewind();
  tapenum++;
}

rewind()
{
  register struct cab_move *WUB;

  WUB=(struct cab_move *)TAPECAB;
  WUB->cab_segmentnum = 0;
  if (satapego(CAB_UTIL,CAB_MOVE)) {
    printf("couldn't rewind tape\n");
    return(-1);
    }
}

tapestrat(dir,blk,nblks)
char dir;
long nblks,blk;
{
  register struct cab_rw *WUB;

  if ((dir != 'R') && (dir != 'B')) {
	printf("Illegal mode %c\n",dir);
	exit(1);
	}
  WUB=(struct cab_rw *)TAPECAB;
  WUB->cab_nblocks = nblks;
  WUB->cab_disknum=unit;
  WUB->cab_blkno=blk;
  if (satapego(CAB_MV,(dir=='B'?CAB_SAVE : CAB_RECALL))) {
    printf("fatal io error stat=%x, err=%x\n",
	WUB->cab_estat,reterr);
    exit(1);
    }
}

satapego(class,op)
unsigned char class,op;
{
  register cab_t *WUB;
  long tocnt,delay;
  
  WUB=(cab_t *)TAPECAB;
  WUB->cab_class=class;
  WUB->cab_op=op;
  WUB->cab_flag=B_CABGO;
  out_multibus(IMSCWU,B_M_CMD);
  for (tocnt=18500000;tocnt && (WUB->cab_flag == B_CABGO);tocnt--) ;
  for (delay=100L;delay;delay--) ;
  out_multibus(IMSCWU,B_M_INTCLR);
  return(!tocnt || (WUB->cab_flag == B_CABERR));
}
