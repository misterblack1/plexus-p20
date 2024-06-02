/* send.h */


/* This information is proprietary and is the property of Bell
   Telephone Laboratories, Incorporated.  Its reproduction or
   disclosure to others, either orally or in writing, is pro-
   hibited without written permission of Bell Laboratories. */

#include <setjmp.h>
#define F 037&

#define LNL 501
#define LNX 502
#define NTB 22
#define NDR 12
#define DBR (1<<8)

#define BCD 1
#define ASC 2
#define SIG 4

#define ENG 1
#define ETB 2
#define EDT 4
#define ELL 8
#define EEX 16

#define SLS 0141
#define STR 0134
#define DOL 0133

#define TFIL  0
#define TCOM  1
#define TEOF  2
#define TINP  3
#define TTTY  4
#define TPIN  5
#define TPTY  6
#define TSFL  7
#define TRFL  8
#define TPKY  9
#define TDHX 10
#define TDKY 11
#define TEXC 12
#define TDOL 13
#define TFMT 14
#define TMSG 15
#define TVFL 16
#define TCHD 17
#define TQHX 18
#define TQKY 19
#define TQPK 20

#define ESPIPE 29

extern NHST;

#define NLIN 8
#define NLIX 600

struct iobf
 {int fd,bc,bb;
  char *bp,bf[512];};

struct format
 {int ffd,ffe,ffm,ffs,fft;
  char ftab[NTB];};

struct keywd
 {struct keywd *nx,*lk;
  int nk,nr;
  char *kp,*rp;};

struct context
 {struct context *ocx;
  char flg[32];
  struct format *dsp;
  struct keywd *kw0,*kw1;
  char *nam;
  int lno,lvl,typ;
  int sfd;
  char stm[24];
  int dfl,dsv[NDR];};

struct trap
 {struct trap *str;
  jmp_buf save;};

extern	struct	trap	*tchain;

struct hsts
 {int hcnt;
  char *hnam;};

int abort,error;

int didf;

int code,maxcol;

int	qat,cue;
long	cnt;

int tin,oed,tty;

char *qer,*tmpf,*home;

char ss[LNX];

extern char msglvl[];

struct iobf obf,dbf,qbf,tbf;

struct keywd *ikw,*kwx[256];

int nhst,seqflg;
char hstname[];


char trt[256],rtr[256];

/*end*/
