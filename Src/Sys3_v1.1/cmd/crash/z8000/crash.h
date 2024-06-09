#include  "stdio.h"
#include  "a.out.h"
#include  "sys/param.h"
#include  "sys/dir.h"
#include  "sys/user.h"
#include  "sys/var.h"

#define CLICKSIZE 2048

struct	uarea	{
	struct	user	u;
	char	sysstk[USIZE * CLICKSIZE - sizeof u];
} ;

struct	frame	{
	unsigned  f_r5;
	unsigned  f_ret;
} ;

struct	glop	{
	int	g_x0;
	int	g_x1;
	int	g_r0;
	int	g_r1;
	int	g_r2;
	int	g_r3;
	int	g_r4;
	int	g_r5;
	int	g_sp;
	unsigned  int  g_ka6;
} ;
struct	tsw	{
	char	*t_nm;
	int	t_sw;
	char	*t_dsc;
} ;

struct	prmode	{
	char	*pr_name;
	int	pr_sw;
} ;

#define	STKMASK	0140000
#define	STKHI	0170000
#define MAXI	30

extern unsigned maps[];
extern	long	delta;
extern	struct	var	v;
extern	int	kmem;
extern	long	ka6;
extern	struct	nlist	*File, *Inode, *Mount, *Swap, *Core, *Proc,
			*Sys, *Time, *Panic, *Etext, *Text, *V,
			*Buf, *End, *Callout, *Lbolt;

#define	STACK	1
#define	UAREA	2
#define	FILES	3
#define	TRACE	4
#define	QUIT	5
#define	KA6	6
#define	INODE	7
#define	MOUNT	8
#define	TTY	9
#define	Q	10
#define	TEXT	11
#define	TNS	13
#define	DS	14
#define	PROC	15
#define	STAT	16
#define	R5	17
#define	BUFHDR	20
#define	BUFFER	21
#define	TOUT	22
#define	NM	23
#define	OD	24
#define	MAP	25
#define	VAR	28

#define	DIRECT	2
#define	OCTAL	3
#define	DECIMAL	4
#define	CHAR	5
#define	WRITE	6
#define	INODE	7
#define	BYTE	8
#define HEX	9

