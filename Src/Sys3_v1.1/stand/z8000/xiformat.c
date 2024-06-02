#include "sys/param.h"
#include "sys/plexus.h"
#include "sys/imsc.h"

#define IMSCWU 0x1f

#define reterr ( (WUB->cab_etype << 8) | (WUB->cab_ecode))

#define MAXUNIT 3
#define MINUNIT 0
#define MINCYLS 3
#define MAXCYLS 1024
#define MINHEADS 1
#define MAXHEADS 24
#define MINBYTES 32
#define MAXBYTES 1024
#define MINSECTS 18
#define MAXSECTS 34
#define MINALTS 0
#define MAXALTS 50
#define MININTERLV 1
#define MAXINTERLV 10
#define MINFSBS 512
#define MAXFSBS 1024

/* following should be (MAXALTS*MAXHEADS) but not enough memory for
   table and can't get split i/d standalone programs to load */
#define NBADTRKS 150

#define NORMAL 0

extern char debug;
char verify;

long atol();
unsigned char unit;
unsigned char maxhead;
short maxcyl;
short diagcyl;
unsigned char maxsect;
unsigned char head;
short sectsize;
unsigned char sect;
short numaltcyls;
short hicyl;
short cyl;
short listcyl;
unsigned char listhd;
char initbuf[1024];
short maxalts;
short altcyl;
unsigned char althd;
short numalts;

struct badtrack {
  short cyl;
  short altcyl;
  unsigned char hd;
  unsigned char althd;
  } badtrack[NBADTRKS];


/*
 *  getnum prompts for a response with optional limits
 *  dislayed and loops until a valid response has been
 *  entered. to turn off limits call with min and max
 *  set to -1
 */
getnum(prompt,nmin,nmax)
char *prompt;
short nmin,nmax;
{
  char buf[80];
  short i;

  do {
    do {
      printf("%s?",prompt);
      if (~(nmin&nmax))
        printf(" [%d-%d]",nmin,nmax);
      printf(": ");
      gets(buf);
      if (buf[0]=='?') debug++;
      if (buf[0] == '!') verify++;
      }
    while ( (buf[0]=='?') || (buf[0]=='!') );
    i=(short)atol(buf);
    }
  while ( (~(nmin&nmax)) && ((i<nmin) || (i>nmax)) );
  return(i);
}

cylcompare(a,b)
register struct badtrack *a,*b;
{
	if (a->cyl > b->cyl) return(1);
	else if (a->cyl == b->cyl) return(0);
	else return(-1);
}

hdcompare(a,b)
register struct badtrack *a,*b;
{
	if (a->hd > b->hd) return(1);
	else if (a->hd == b->hd) return(0);
	else return(-1);
}

igetinitinfo()
{
  char buf[20];

  maxcyl = getnum("Total number of cylinders",MINCYLS,MAXCYLS);
  diagcyl = maxcyl-1;
  listcyl = maxcyl-2;
  listhd = 0;
  getnum("Number of heads on removable",0,0);
  maxhead = (unsigned char)getnum("Number of heads on fixed",MINHEADS,MAXHEADS);
  sectsize = getnum("Data bytes per sector",MINBYTES,MAXBYTES);
  maxsect = (unsigned char)getnum("Sectors per track",MINSECTS,MAXSECTS);
  numaltcyls = getnum("Number of alternate cylinders",MINALTS,MAXALTS);
  maxalts=numaltcyls*maxhead;
  hicyl=maxcyl-numaltcyls-3;
  getnum("Interleave factor",MININTERLV,MAXINTERLV);
  printf("User pattern? [8 hex digits]: "); 
  gets(buf);
}

getdefects()
{
  short bcyl;
  unsigned char bhd;
  register short i;

  initttbl();
  for (i=1;i<maxalts;i++) {
    if (getbtrk(&bcyl,&bhd)) return;
    if (chbdup(bcyl,bhd)) {
      printf("duplicate bad track\n");
      }
    else {
      entbtrk(bcyl,bhd);
      }
    }
}

initttbl()
{
  register short i;

  numalts=1;
  altcyl=maxcyl-3;
  althd=maxhead-1;
  for (i=0;i<NBADTRKS;i++)
    badtrack[i].cyl=badtrack[i].altcyl= -1;
}

getbtrk(c,h)
register short *c;
register unsigned char *h;
{
  char buf[20];
  short i;

  do {
    printf("Defective tracks on fixed?(cyl,head) [0-%d,0-%d]: ",
	maxcyl-1,maxhead-1);
    gets(buf);
    if (*buf == '\0') return(1);
    *c =(short)atol(buf);
    if (*c > (maxcyl-1))
	printf("Cylinder out of range.\n");
    for(i=0; (buf[i] != ',') && (i<20); i++);
    *h = atol(&buf[i+1]);
    if (*h > maxhead-1) {
	printf("Head out of range.\n");
	continue;
    }
  }
  while ( (*c < 0) || (*c > (maxcyl-1)) || (*h > (maxhead-1)) || (*h < 0) );
  return(0);
}

chbdup(c,h)
register short c;
unsigned char h;
{
  short i;

  for (i=1;i<numalts;i++) 
    if ( (badtrack[i].cyl == c) &&
	 (badtrack[i].hd  == h) ) return(1);
  return(0);
}

entbtrk(c,h)
register short c;
unsigned char h;
{
  badtrack[numalts].cyl=c;
  badtrack[numalts].hd=h;
  numalts++;
}

assignalt(c,h)
register short *c;
unsigned char *h;
{
	if (altcyl <= hicyl)
	{
	printf("TOO MANY BAD TRACKS\n");
	exit(1);
	}
  *c=altcyl;
  *h=althd;
	if (althd == 0)
	{
		althd = maxhead-1;
		altcyl--;
	}
	else
	{
		althd--;
	}
}

printbad()
{
	register short i;

	for (i=1;i<numalts;i++)
	{
		printf("%d(0x%x)  %d(0x%x)\t\t%d(0x%x)  %d(0x%x)\n",
			badtrack[i].cyl,
			badtrack[i].cyl,
			badtrack[i].hd,
			badtrack[i].hd,
			badtrack[i].altcyl,
			badtrack[i].altcyl,
			badtrack[i].althd,
			badtrack[i].althd);
	}
}
sortalttbl()
{
	/* sort by head then cyl to get cyl-hd sorted spares list */
	/* this assumes that qsort is a stable sort */
	qsort(&badtrack[1].cyl,numalts-1,sizeof(struct badtrack),hdcompar);
	qsort(&badtrack[1].cyl,numalts-1,sizeof(struct badtrack),cylcompar);
}

installdefects()
{
	register short i;

	sortalttbl();
	for (i=1;i<numalts;i++)
	{
		if ( (badtrack[i].cyl > hicyl) &&
		     ((badtrack[i].cyl != listcyl) ||
		       (badtrack[i].hd != listhd)) ) /* spare spares track */
			continue; /* don't spare trk in alt area */
				  /* they won't be used */
		assignalt(&badtrack[i].altcyl,&badtrack[i].althd);
		while (chbdup(badtrack[i].altcyl,badtrack[i].althd))
		{
			assignalt(&badtrack[i].altcyl,&badtrack[i].althd);
		}
	}
if (debug) printbad();
}
		
setblk0()
{
  short i;

  for (i=0;i<sizeof(initbuf);i++)
    initbuf[i]=0;

  ((short *)initbuf)[0] = maxcyl;
  initbuf[3] = maxhead;
  initbuf[4] = sectsize & 0xff;
  initbuf[5] = maxsect;
  initbuf[6] = numaltcyls;
  initbuf[7] = sectsize >> 8;
  ((short *)initbuf)[5] = 'pd';
}

getblk0()
{
  maxcyl = ((short *)initbuf)[0];
  maxhead = initbuf[3] ;
  sectsize = initbuf[4] | (initbuf[7] << 8);
  maxsect = initbuf[5] ;
  numaltcyls = initbuf[6] ;
  diagcyl = maxcyl-1;
  listcyl = maxcyl - 2;
  listhd = 0;
  maxalts=numaltcyls*maxhead;
  hicyl = maxcyl-numaltcyls-3;
}

igetfsinfo()
{
  ((short *)initbuf)[4] = getnum("File system blocksize",MINFSBS,MAXFSBS);
  for (;;) {
    printf("Default boot name? : ");
    gets(&initbuf[16]);
    if (initbuf[16] != '\0') break;
    }
}

imain(buf)
char buf[];
{
  short i,status;
  long blk;
  char tbuf[4];

  for (i=10;i<50;i++)
    if (buf[i] == '(') break;
  unit = atoi(&buf[i+1]);

  switch (buf[0])
  {
  case 'f':
  {
  igetinitinfo();
  getdefects();
  installdefects();
  setblk0();
  igetfsinfo();

  xdef_pddisk();
  recal_disk();
  while ( fmt_track(maxcyl-1,0) != NORMAL)
  {
    printf("make disk ready, press return ");
    gets(tbuf);
    recal_disk();
  }
  printf("formatting\n");
  if ((status=fmt_disk()) != NORMAL) {
    printf("fmt err=%x\n",status);
    exit(1);
    }
  recal_disk();
  write_inittab();
  write_spares();
  printf("testing");if (verify) printf(" and verifying");printf("\n");
  test_disk('y');
  write_inittab();
  write_spares();
  printf("\nDisk formatted and initialized successfully!\n");
  exit(0);
  }
    case 'r':
    {
    ginit();
    verify=0;
    test_disk('n');
    printf("Disk reads complete\n");
    exit(0);
    }

    case 'l':
    {
    ginit();
    printbad();
    exit(0);
    }

    case 's':
    {
    ginit();
    while (!getbtrk(&cyl,&head))
    {
	sp_trk(cyl,head);
    }
    write_spares();
    exit(0);
    }

    default:
    {
    printf("Illegal option for %s\n",&buf[10]);
    exit(1);
    }

    }
}

ginit()
{
    set_defs();
    xdef_pddisk();
    recal_disk();
    read_inittab();
    getblk0();
    xdef_pddisk();
    read_spares();
    }

read_inittab()
{
  short status;
  if ((status=read_disk(0L,(long)sizeof(initbuf),
	 MBSDSEG,&initbuf[0])) != NORMAL)
	 {
	 printf("blk0 rd err stat=%x\n",status);
	 exit(1);
	 }
}

write_inittab()
{
  write_disk(0L,(long)sizeof(initbuf),
	 MBSDSEG,&initbuf[0]);
}

read_spares()
{
  short status;
  if ((status=read_disk((long)(maxcyl-2)*maxhead*maxsect,
	(long)sizeof(badtrack),MBSDSEG,badtrack)) != NORMAL)
  {
  printf("sp rd err stat=%x\n",status);
  exit(1);
  }
  numalts = badtrack[0].cyl ;
  altcyl = badtrack[0].altcyl;
  althd = badtrack[0].althd;
}

write_spares()
{
  badtrack[0].cyl = numalts;
  badtrack[0].altcyl = altcyl;
  badtrack[0].althd = althd;
  write_disk((long)(maxcyl-2)*maxhead*maxsect,
	(long)sizeof(badtrack),MBSDSEG,badtrack);
}

set_defs()
{
	maxsect = 34;
	maxhead = 3;
	maxcyl = 20;
}


fmt_disk()
{
  short status;

  for (cyl=0;cyl<maxcyl;cyl++)
    for (head=0;head<maxhead;head++)
      if ((status=fmt_track(cyl,head)) != NORMAL) return(status);
  return(NORMAL);
}

e5check(buf,y)
char *buf;
char y;
{
	short p;

	if ( (debug || verify) && ((y=='y')||(y=='Y')) )
	{
	for (p=0;p<maxsect*512;p++)
		if (*buf++ != (char)0xe5) return(1);
	}
	return(0);
}
		
char bbuf[MAXSECTS][512];

chk_track(c,h,e5,w)
register short c;
register char h;
char e5,w;
{


#ifdef DEBUG
	printf("ck trk %d/%d\n",c,h);
#endif
	if ( (read_disk((long)c*h*maxsect,(long)maxsect*512,
		MBSDSEG,bbuf) != NORMAL) ||
	     (e5check(bbuf,e5) ))
	     {
	     	if (w == 'y')
	     	{
	     		sp_trk(c,h);
	     		return(1);
	     	}
		else
		{
	        printf("bad trk %d/%d\n",c,h);
		return(0);
		}
	     }
	     else 
	     {
	     return(0);
	     }
}

sp_trk(c,h)
register short c;
register char h;
{
	register short i;

	     if (chbdup(c,h))
	     {
#ifdef DEBUG
		printf("chk_track: chbdup found trk in tbl\n");
#endif
	       for (i=1;i<numalts;i++)
	       { 	if ( (badtrack[i].cyl == c) &&
				(badtrack[i].hd == h) ) break;
	       }
#ifdef DEBUG
	       if (i==numalts) 
	       {
		printf("fatal internal err\n");
		exit(1);
	       }
#endif
	       assignalt(&badtrack[i].altcyl,&badtrack[i].althd);
	       fmt_track(badtrack[i].altcyl,badtrack[i].althd);
	     }
	     else 
	     {
#ifdef DEBUG
		if (debug)
		{
			printf("chk_track: new bad track\n");
			printf("numalts=%d\n",numalts);
		}
#endif
		entbtrk(c,h);
	     	assignalt(&badtrack[numalts-1].altcyl,
			&badtrack[numalts-1].althd);
		fmt_track(badtrack[numalts-1].altcyl,
			badtrack[numalts-1].althd);
	     }
	     sortalttbl();
#ifdef DEBUG
	     printbad();
#endif
	     fmt_track(c,h);
	     printf("track %d/%d spared\n",c,h);
}

test_disk(w)
char w;
{
  short status;
  char buf[6];

  buf[0]='n';
#ifdef DEBUG
  if(debug)
  {
	printf("test_disk\n");
	printf("verify e5's (y/n)? ");
	gets(buf);
  }
#endif
  for (cyl=0;cyl<=hicyl;cyl++)
    for (head=0;head<maxhead;head++)
      while(chk_track(cyl,head,buf[0],w));
  cyl=listcyl;head=listhd; /* test spares track */
  while (chk_track(cyl,head,buf[0],w));
  return(NORMAL);
}

fmt_track(c,h)
register short c;
unsigned char h;
{
  register short ac;
  unsigned char ah;
  register short i;
  unsigned char f;

  ac=c;
  ah=h;
  f=CAB_FMT;
  if (c > hicyl )
  {	/* trk is beyond user trk area. default id to unreachable
	   trk so if alt is unused or bad itself it will
	   be unreachable */
	ac = MAXCYLS+1;
  }
  if ( (c==listcyl) && (h==listhd) )
  { /* special case for the spares list track which
       is beyond hicyl but must be usable. if it is
       bad it will be spared below */
	ac = c;
  }
  for (i=1;i<numalts;i++) {
    if ( (badtrack[i].cyl==c) &&
	 (badtrack[i].hd ==h) )  /* at a bad track */
	   {
	   if ( (c >hicyl) && ((c!=listcyl) || (h!=listhd)) )
				/* above handles sparing spares list trk */
	   {
		break;   /* fmt bad trk in spare area as
				unreachable cyl number */
	   }
	   ac=badtrack[i].altcyl; /* fmt bad track with id of alt */
	   ah=badtrack[i].althd;
	   f=CAB_ALTFMT;
	   break;
	   }
    if ( (badtrack[i].altcyl==c) &&
	 (badtrack[i].althd ==h) ) { /* at an alt track */
	   ac=badtrack[i].cyl;
	   ah=badtrack[i].hd; /* fmt with id of trk it is alt for */
	   f=CAB_FMT;
	   break;
	   }
  }	
  return(fmt_it(f,c,h,ac,ah));
}
xdef_pddisk()
{
  struct cab_def *WUB;
  
  WUB=(struct cab_def *)WUA;
  WUB->cab_class=CAB_MAINT;
  WUB->cab_op=CAB_DEF;
  WUB->cab_disknum=unit;
  WUB->cab_sectnum=maxsect;
  WUB->cab_headnum=maxhead;
  WUB->cab_cylnum=maxcyl;
  WUB->cab_altcylnum=diagcyl;
  if (disk_go())
    {
    printf("def disk err stat=%x\n",reterr);
    exit(1);
    }
}

fmt_it(f,pcyl,phd,acyl,ahd)
unsigned char f;
short pcyl,acyl;
unsigned char phd,ahd;
{
  struct cab_def *WUB;

#ifdef DEBUG
  printf("fmt %d/%d as %d/%d %c\n",pcyl,phd,acyl,ahd,
	(f==CAB_FMT)?' ':'S');
#endif
  WUB = (struct cab_def *)WUA;
  WUB->cab_class=CAB_MAINT;
  WUB->cab_op=f;
  WUB->cab_disknum=unit;
  WUB->cab_headnum=phd;
  WUB->cab_cylnum=pcyl;
  WUB->cab_altheadnum=ahd;
  WUB->cab_altcylnum=acyl;
  if (disk_go())
    return(reterr);
  else
    return(NORMAL);
}

recal_disk()
{
  struct cab_seek *WUB;

  WUB = (struct cab_seek *)WUA;
  WUB->cab_disknum=unit;
  WUB->cab_class=CAB_UTIL;
  WUB->cab_op=CAB_DKRECAL;
  if (disk_go())
    {
    printf("recal err stat=%x\n",reterr);
    exit(1);
    }
}


/*
seek_pd(blk)
long blk;
{

  struct cab_seek *WUB;

  WUB = (struct cab_seek *)WUA;
  WUB->cab_blocknum=blk;
  WUB->cab_disknum=unit;
  WUB->cab_class=CAB_UTIL;
  WUB->cab_op=CAB_SEEK;
  if (disk_go())
    {
    printf("seek err stat=%x\n",reterr);
    exit(1);
    }
}
*/

write_disk(blk,nbytes,buf)
long blk,nbytes;
long buf;
{
  short status;
  struct cab_rw *WUB;

/*  seek_pd(blk); */

  WUB = (struct cab_rw *)WUA;
  WUB->cab_nblocks=nbytes;
  WUB->cab_blkno=blk;
  WUB->cab_disknum=unit;
  WUB->cab_bufaddr=buf;
  WUB->cab_class=CAB_IO;
  WUB->cab_op=CAB_WRITE;

  if (disk_go())
    {
    printf("write err stat=%x\n",reterr);
    exit(1);
    }
}

read_disk(blk,nbytes,buf)
long blk,nbytes;
long buf;
{
  short status;
  struct cab_rw *WUB;

/*  seek_pd(blk); */

  WUB = (struct cab_rw *)WUA;
  WUB->cab_nblocks=nbytes;
  WUB->cab_blkno=blk;
  WUB->cab_disknum=unit;
  WUB->cab_bufaddr=buf;
  WUB->cab_class=CAB_IO;
  WUB->cab_op=CAB_READ;

  if (disk_go())
    {
    return(reterr);
    }
    else return(NORMAL);
}


/*
pd_rdy()
{
	struct cab_seek *WUB;

	WUB=(struct cab_seek *)WUA;
	WUB->cab_disknum = unit;
	WUB->cab_class=CAB_UTIL;
	WUB->cab_class=CAB_DKRDY;
	if (disk_go())
	{
		return(reterr);
	}
	else return(NORMAL);
*/

disk_go()
{

  long tocnt,delay;
  cab_t *WUB;

  WUB = (cab_t *)WUA;
  WUB->cab_flag = B_CABGO;
  out_multibus(IMSCWU,B_M_CMD);
  for (tocnt=125000; tocnt && (WUB->cab_flag == B_CABGO); tocnt--) ;
  for (delay=100L;delay;delay--) ;
  out_multibus(IMSCWU,B_M_INTCLR);
  return(!tocnt || (WUB->cab_flag == B_CABERR));
}

