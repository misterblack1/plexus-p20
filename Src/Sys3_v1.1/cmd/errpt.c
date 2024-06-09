/* Format and interpret the error log file */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/map.h>
#include <sys/elog.h>
#include <sys/err.h>
#include <sys/erec.h>
#include <time.h>

#define writout()	((page<=limit) && (mode==PRINT))
#define major(x)	(int)((unsigned)(x>>8)&0377)
#define minor(x)	((int)x&0377)
#define araysz(x)	((sizeof(x)/sizeof(x[0]))-1)
#define readrec(x)	(fread(&ercd.ebb.block,\
			(e_hdr.e_len - sizeof(struct errhdr)),1,x) )

char Nl [1];
#ifdef m68
#define FORM "%08x"
#define FORM2 "%08x\n"
#else
#define FORM "%04x"
#define FORM2 "%08lx\n"
#endif
#define NULS	 Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl

#define INAUG "0301080077"	/* roughly the inception of error logging [mmddhhmmyy] */
#define WDLEN 16
#define MINREC 8
#define MAXREC 74
#define DEVADR ((physadr)(0160000))
#define MAXLEN 66
#define MAXSTR 40
#define NMAJOR 16
#define PGLEN 60
#define MBAREG 5
#define INT  15

#define MEM  14
#define YES 1
#define NO 0
#define PRINT 1
#define NOPRINT 0
#define DSEC 3	/* 2**DSEC is the number of logical
			partitions on RP03/4/5/6 as defined
			in the device driver. */

/* Space needed for each block device in detail report. */
/* CCF need size for report */
int pneed[6] = {0,0,0,0,0,0};

/* NMAJOR devices of 6 possible logical units */
struct sums {
	long	soft;
	long	hard;
	long	totalio;
	long	misc;
	long	missing;
} sums[NMAJOR][6];

union ercd {
	struct  estart start;
	struct	eend end;
	struct	etimchg timchg;
	struct	econfchg confchg;
	struct	estray stray;
	struct	eccerr ecc;
	struct  eb {
		struct	eblock block;
		unsigned short reginf [30];
	} ebb;
} ercd;

struct errhdr e_hdr;


int dmsize [];
int MAJ;
int MIN;
int page=1;
int print;
int mode = NOPRINT;
int line;
int n = 0;
int aflg;
int dflg;
int fflg;
int Unix = 1;
int parsum;
int eccsum;
int straysum;
int limit = 10000;
int optdev = 0;
long readerr = 0;
FILE	*file;
time_t	atime;
time_t	stime = 0L;
time_t	etime = 017777777777L;
time_t	fftime = 017777777777L;
time_t	ltime = 0L;
char interp [MAXSTR];
char choice [MAXSTR];
long tloc;
char cbuf[];
#ifdef vax
struct vaxreg {
	char *regnam;
	char *bitcod [WDLEN*2];
};
#endif
struct regs {
	char *regname;
	char *bitcode [WDLEN];
};

char *htime[20];
char *header = "SYSTEM ERROR REPORT";
char *hd1 = "System Error Report - Selected Items";
char *hd2 = "Summary Error Report";
/* Must be same order as "true" major device numbers */
char *dev[] = {
	"DK",
	"MT",
	"PD",
	"PT",
	"IS",
	"RM",
	0
};
/* Register names and bit mnemonics for each */
/* register and bit to be interpreted on detail report. */

struct regs dkregs [] = {
	0
};

struct regs mtregs [] = {
	0
};

struct regs pdregs [] = {
	0
};

struct regs ptregs [] = {
	0
};

struct regs isregs [] = {
	0
};

struct regs rmregs [] = {
	0
};

#ifdef PDP11
struct regs rkregs [] = {
	"RKDS",Nl,Nl,Nl,Nl,Nl,"WPS",Nl,Nl,Nl,"SIN","DRU",
		Nl,Nl,Nl,Nl,Nl,
	"RKER","WCE","CSE","","","","NXS","NXC","NXD","TE","DLT",
		"NXM","PGE","SKE","WLO","OVR","DRE",
	"RKCS",NULS,
	"RKWC",NULS,
	"RKBA",NULS,
	"RKDA",NULS,
	0
};
struct regs rpregs[] = {
	"RPDS",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"SUFU",Nl,
		"SUSI","HNF",Nl,Nl,Nl,
	"RPER","DSKERR","EOP","NXME","WCE","TIMEE","CSME",
		"WPE","LPE","MODE","FMTE","PROG","NXS","NXT","NXC","FUV","WPV",
	"RPCS",NULS,
	"RPWC",NULS,
	"RPBA",NULS,
	"RPCA",NULS,
	"RPDA",NULS,
	0
};
struct regs rfregs[] = {
	"RFCS",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"MXF","WLO",
		"NED","DPE","WCE",Nl,Nl,
	"RFWC",NULS,
	"RFBA",NULS,
	"RFDA",NULS,
	"RFDAE",Nl,Nl,Nl,Nl,Nl,"DAOVFL",Nl,"DRL",Nl,Nl,"NEM",
		"CTER","BTER","ATER","APE",
	0
};
struct regs tmregs [] = {
	"TMER",Nl,Nl,"WRL",Nl,Nl,Nl,Nl,"NXM","BTE","RLE","EOT",
		"BGL","PAE","CRE",Nl,"ILL",
	"TMCS",NULS,
	"TMBC",NULS,
	"TMBA",NULS,
	"Z   ",NULS,
	"Z   ",NULS,
	0
};
struct regs tcregs [] = {
	"TCCSR",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"NEX","DATM","BLKM",
		"SELE","ILO","MTE","PAR",Nl,
	"TCCM",NULS,
	"TCWC",NULS,
	"TCBA",NULS,
	"Z   ",NULS,
	0
};
#endif
#ifdef vax
struct regs hpregs[] = {
	"HPCS1","GO","F0","F1","F2","F3","F4",Nl,Nl,
		Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
	"HPDS",Nl,Nl,Nl,Nl,Nl,Nl,"VV","DRY",
		"DPR",Nl,Nl,Nl,"MOL","PIP","ERR","ATA",
	"HPER1",NULS,
	"HPMR",NULS,
	"HPAS","ATA0","ATA1","ATA2","ATA3","ATA4","ATA5","ATA6","ATA7",
		Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
	"HPDA",NULS,
	"HPDT",NULS,
	"HPLA",NULS,
	"Z   ",NULS,
	"HPOF",NULS,
	"HPDC",NULS,
	"HPCC",NULS,
	"HPER2",NULS,
	"HPER3",NULS,
	"HPEC1",NULS,
	"HPEC2",NULS,
	0
};
struct regs htregs[] = {
	"HTCS1","GO","F0","F1","F2","F3","F4",Nl,Nl,
		Nl,Nl,Nl,"DVA",Nl,Nl,Nl,Nl,
	"HTDS","SLA","BOT","TM","IDB","SDWN","PES","SSC","DRY",
		"DPR",Nl,"EOT","WRL","MOL","PIP","ERR","ATA",
	"HTER1","ILF","ILR","RMR","CPAR","FMT","DPAR","INC/VPE",
		"PER/LRC","NSG","FCE","CS/ITM","NEF","DTE","OPI","UNS",
		"COR/CRC",
	"HTMR",NULS,
	"HTAS","ATA0","ATA1","ATA2","ATA3","ATA4","ATA5","ATA6",
		"ATA7",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
	"HTFC",NULS,
	"HTDT",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"SPR",Nl,Nl,Nl,Nl,Nl,
	"HTCK",NULS,
	"HTSN",NULS,
	"HTTC","SS0","SS1","SS2","EVPAR","FMT0","FMT1","FMT2",
		"FMT3","DEN0","DEN1","DEN2",Nl,"EAODTE","TCW","FCS",
		"ACCL",
	0
};
struct vaxreg mbareg[] = {
	"MBACSR",NULS,Nl,Nl,Nl,Nl,Nl,"OTMP","APU","APD",Nl,Nl,
		"XDF","MXE",Nl,"UXRDE","WDSE","SBIPE",
	"MBACR","INIT","ABRT","IE","MM",Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,Nl,Nl,Nl,NULS,
	"MBASR","RDTO","IFSQTO","RDSUB","ERCON","UNVMP","MPPAR","MBDPAR",
		"MBEXC","MXF","WCLE","WCVE","DTL","DTABT","DTCOMP",Nl,
		Nl,"MATA","MCWE","NED","PGE",Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,Nl,"CRD","NRC","DTB",
	"MBAVAR",NULS,NULS,
	"MBABCR",NULS,NULS,
	0
};
#endif
#ifdef pdp11
struct regs hpregs[] = {
	"HPCS1",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,"MCPE","TRE",Nl,
	"HPWC",NULS,
	"HPBA",NULS,
	"HPDA",NULS,
	"HPCS2",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"MDPE","MXF","PGE",
		"NEM","NED","PE","WCE","DLT",
	"HPDS",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,"WRL","MOL",Nl,Nl,Nl,
	"HPER1","ILF","ILR","RMR","PAR","FER","WCF","ECH","HCE",
		"HCRC","AOE","IAE","WLE","DTE","OPI","UNS","DCK",
	"HPAS",NULS,
	"Z   ",NULS,
	"HPDB",NULS,
	"HPMR",NULS,
	"HPDT",NULS,
	"Z   ",NULS,
	"HPOF",NULS,
	"HPDC",NULS,
	"HPCC",NULS,
	"HPER2",NULS,
	"HPER3",NULS,
	"HPEC1",NULS,
	"HPEC2",NULS,
	"HPBAE",NULS,
	"HPCS3",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,Nl,Nl,"APE",
	0
};
struct regs htregs [] = {
	"HTCS1",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,Nl,"MCPE","TRE",Nl,
	"HTWC",NULS,
	"HTBA",NULS,
	"HTFC",NULS,
	"HTCS2",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"MDPE","MXF","PGE",
		"NEM","NED","PE","WCE","DLT",
	"HTDS",Nl,"BOT",Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,"EOT","WRL","MOL",Nl,Nl,Nl,
	"HTER","ILF","ILR","RMR","CPAR","FMT","DPAR",
		"INC/VPE","PEF/LRC","NSG","FCE","CS/ITM","NEF",
		"DTE","OPI","UNS","COR/CRC",
	"HTAS",NULS,
	"HTCK",NULS,
	"HTDB",NULS,
	"HTMR",NULS,
	"HTDT",NULS,
	"Z   ",NULS,
	"HTTC",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"NRZ",
		"PE",Nl,Nl,Nl,Nl,Nl,
	"HTBAE",NULS,
	"HTCS3",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,Nl,Nl,"APE",
	0
};
struct regs hsregs[] = {
	"HSCS1",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,"MCPE","TRE","SC",
	"HSWC",NULS,
	"HSBA",NULS,
	"HSDA",NULS,
	"HSCS2",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"MDPE","MXF","PGE",
	"NEM","NED","PE","WCE","DLT",
	"HSDS",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,Nl,"WRL","MOL",Nl,Nl,Nl,
	"HSER","ILF","ILR","RMR","PAR",Nl,Nl,Nl,Nl,Nl,"AO",
		"IAE","WLE","DTE","OPI","UNS","DCK",
	"HSAS",NULS,
	"Z   ",NULS,
	"HSDB",NULS,
	"HSMR",NULS,
	"Z   ",NULS,
	"HSBAE",NULS,
	"HSCS3",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,
		Nl,Nl,Nl,Nl,Nl,"APE",
	0
};
struct regs rlregs[] = {
	"RLCS",Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,Nl,"OPI","CRC","DLT/HNF","NXM","DE",Nl,
	"RLBS",NULS,
	"RLDA",NULS,
	"RLMP",Nl,Nl,Nl,"BH","HO","CO","HS",Nl,"DSE","VC","WGE","SPE",
		"SKTO","WL","CHE","WDE",
	0
};
#endif
int dkblk();
int mtblk();
int pdblk();
int ptblk();
int isblk();
int rmblk();

/* Device specification functions */
/* Must remain in same order as true major device numbers */
int (*func[]) () = {
	dkblk,
	mtblk,
	pdblk,
	ptblk,
	isblk,
	rmblk,
	0
};
struct regs  *devregs[] = {
	dkregs,
	mtregs,
	pdregs,
	ptregs,
	isregs,
	rmregs,
	0
};
struct pos {
	unsigned flg;
	unsigned unit;
	unsigned cyl;
	unsigned trk;
	unsigned sector;
} pos;
/* Array order directed by MMR3 */
/* CCF need to relate MMR3 to something */
char *msg[] = {
	0
};


char *lines [] = {
/* 0*/	"\n",
/* 1*/	"%s     ERROR LOGGED ON   %s\n",
/* 2*/	"       Physical Device                  %u\n",
/* 3*/	"       Logical Device                   %d (%2.2o)\n",
/* 4*/	"       Device Address                   ",
/* 5*/	"       Retry Count                      %u\n",
/* 6*/	"       Error Diagnosis                  %s\n",
/* 7*/	"       Simultaneous Bus Activity        ",
/* 8*/	"       Registers at Error time\n",
/* 9*/	"               %s          ",
/*10*/	"       Physical Buffer Start Address         ",
/*11*/	"       Transfer Size in Bytes                     %11u\n",
/*12*/	"       Type of Transfer                              %8s\n",
/*13*/	"       Block No. in Logical File System      %16ld\n",
/*14*/	"       I/O Type                                      %8s\n",
/*15*/	"       Cylinder Requested              %11u\n",
/*16*/	"       Track Requested                 %11u\n",
/*17*/	"       Sector Requested                %11u\n",
/*18*/	"       Statistics on Device to date:\n",
/*19*/	"               No. of R/W Operations          %16ld\n",
/*20*/	"               No. of Other Operations        %16ld\n",
/*21*/	"               No. of Unrecorded Errors            %11u\n",
/*9b*/	"%o      ",
/*23*/	"       Sector Requested                  %4u-%u\n",
/*24*/  "       Multibus Map Utilization?                            %3.3s\n",
		0
		};

char *xlines [] = {
/* 0*/  "\n\nDEVICE CONFIGURATION CHANGE   - %s\n",
/* 1*/	"     	DEVICE: %s - %s\n",
/* 2*/	"\n\nSTRAY INTERRUPT on %s\n",
/* 3*/  "       For Controller at - ",
/* 4*/	"       At Location     ",
/* 5*/	"\n\nMEMORY SINGLE BIT ECC ERROR\n",
/* 6*/	"       Address of Error - \n\n",
/* 7*/	"                32K BANK    %4d",
/* 8*/	"                BIT         %4d",
/* 9*/	"\n\nTIME CHANGE ***** FROM %s",
/*10*/  "\t\t   TO  %s \n\n\n\n",
/*11*/  "\nERROR LOGGING SYSTEM SHUTDOWN - %s\n\n\n",
/*12*/	"\nERROR LOGGING SYSTEM STARTED - %s \n",
/*13*/	"\n\n	System Profile:\n\n",
/*14*/	"             Z8000 Processor - %d MHZ\n",
/*15*/	"\t     System Memory Size - %ld Bytes\n",
/*16*/	"\n	Configured with:",
/*17*/	"             SYS3/%s  Operating System (%s)\n",
/*18*/  "             %s \n",
	0
};

char *sumlines [] = {
/* 0*/	"\n\n",
/* 1*/	"%s     UNIT  %d   \n",
/* 2*/	"                       Hard Errors             -   %ld\n",
/* 3*/	"                       Soft Errors             -   %ld\n",
/* 4*/	"                       Total I/O Operations    -   %ld\n",
/* 5*/	"                       Total Misc. Operations  -   %ld\n",
/* 6*/	"                       No. of Errors Missed    -   %ld\n",
/* 7*/	"            Total Read Errors                  -   %ld\n",
/* 8*/	"            Total Memory ECC Errors            -   %d\n",
/* 9*/	"            Total Stray Interrupts             -   %d\n",
/*10*/	"       Date of Earliest Entry: %s",
/*11*/	"       Date of Latest   Entry: %s",
/*12*/  "       Error Types: %s\n",
/*13*/  "       Limitations: ",
/*14*/  "                    ",
0
};


/* Correspondence of input requests to major device defines */
struct tab {
	char *devname ;
	int devnum;
};
struct tab dtab[] = {
	"dk",DK0,	"DK",DK0,	"mt",MT0,	"MT",MT0,
	"pd",PD0,	"is",IS0,	"pt",PT0,	"rm",RM0,
	"PD",PD0,	"IS",IS0,	"PT",PT0,	"RM",RM0,
	"MEM",MEM,	"mem",MEM,	"int",INT,	"INT",INT,	
	0,0
};

char *ctime();
main (argc,argv)
char *argv[];
int argc;
{
	register i,j;
	print = NO;
	while (--argc>0 && **++argv =='-') {
		switch (*++*argv) {
		case 's':	/* starting at a specified time */
			header = hd1;
			if((--argc <=0) || (**++argv == '-'))
			error("Date required for -s option",(char *)NULL);
			if(gtime(&stime,*argv))
			 error("Invalid Start time",*argv);
			break;
		case 'e':	/* ending at a specified time */
			header = hd1;
			if((--argc<=0) || (**++argv =='-'))
			error("Date required for -e option\n",(char *)NULL);
			if(gtime(&etime,*argv)) error("Invalid End time.",(char *)NULL);
			break;
		case 'a':	/* print all devices*/
			aflg++;
			mode = PRINT;
			break;
		case 'p':	/* limit total no. of pages */
			if((--argc<=0) || (**++argv == '-'))
				error("Page limit not supplied.\n",(char *)NULL);
			limit = atoi(*argv);
			break;
		case 'f':	/* fatal errors */
			header = hd1;
			fflg++;
			break;

		default:
			if(j=encode(*argv)) {
				optdev = (optdev |= j);
				dflg++;
				header = hd1;
				mode = PRINT;
				if(strlen(choice)) concat(",",choice);
				concat(*argv,choice);
				}
			else
			fprintf(stderr,"%s?\n",argv);
		}
	}
	for(i=0;i<NMAJOR;i++) {
		for(j=0;j<9;j++) {
			sums[i][j].hard = 0;
			sums[i][j].soft = 0;
			sums[i][j].totalio = 0;
			sums[i][j].misc = 0;
			sums[i][j].missing = 0;
		}
	}
	parsum=0;
	eccsum=0;
	straysum=0;
	if(gtime(&atime,INAUG)) error("Invalid INAUG time",INAUG);
	if (argc ==0)
		report("/usr/adm/errfile");
	else while(argc--) 
		report(*argv++);
	printsum();
	putft();
	exit(0);
}
report(fp)
char *fp;
{

	if((file = fopen(fp,"r"))== NULL) {
		fprintf(stderr,"Cannot open %s\n",fp);
		return;
	}
	inithdng();
	if(writout()) puthead(header);
	putdata();
	if(writout()) putft();
	return;
}
putdata()
{
	while(fread(&e_hdr.e_type,sizeof(struct errhdr),1,file)) {
newtry:
		switch(e_hdr.e_type) {
		
		case E_GOTS:
			setime();
			up();
			break;

		case E_GORT:
			setime();
			up();
			break;

		case E_STOP:
			setime();
			down();
			break;

		case E_TCHG:
			setime();
			timecg();
			break;

		case E_BLK:
			setime();
			blk();
			break;
		
		case E_STRAY:
			setime();
			stray();
			break;

#ifdef PDP11
		case E_PRTY:
			setime();
			party();
			break;
#endif

		case E_ECC:
			setime();
			eccerr();
			break;

		case E_CCHG:
			cconfig();
			setime();
			break;
		default:
			fprintf(stderr,"%d\n",e_hdr.e_len);
			fprintf(stderr,"%d\n",e_hdr.e_type);
			readerr++;
			if(recov()) {
				goto newtry;
			}
			fprintf (stderr,"Unrecovered read error.\n");
		}
	}

	return;
}
/* System Startup Record */

up()
{
register int i;

	if (!readrec(file)) {
		fprintf(stderr,"at up = %x MHZ\n",ercd.start.e_cpu/25);
		fprintf(stderr,"%ld\n",ercd.start.e_name.release);
		fprintf(stderr,"%s %s\n",ercd.start.e_name.release,ercd.start.e_name.sysname);
		readerr++;
		return;
	}
	if(writout()) {
		need(14+araysz(msg));
		printf(xlines[12],ctime(&e_hdr.e_time));
		printf(xlines[13]);
		printf(xlines[17],ercd.start.e_name.release,ercd.start.e_name.sysname);
		printf(xlines[14],ercd.start.e_cpu/25);
		if(ercd.start.e_syssize) {
			printf(xlines[15],ercd.start.e_syssize);
			}
		else line--;
		for(i=0;i<=araysz(msg);i++) {
				if(ercd.start.e_mmr3 & (1<<i)){
				printf(xlines[18],msg[i]);
				}
				else line--;
			}
		printf(xlines[16]);
		afix(araysz(dev),(unsigned) ercd.start.e_bconf,dev);
		printf(lines[0]);
	}
}
/* System Shutdown Record */

down()
{
	if(writout()) {
		need(5);
		printf(xlines[11],ctime(&e_hdr.e_time));
	}
}
/* Time Change Record */

timecg()
{
	if(!readrec(file)) {
		readerr++;
		return;
	}
	if(writout()) {
		need(8);
		printf(xlines[9],ctime(&e_hdr.e_time));
		printf(xlines[10],ctime(&ercd.timchg.e_ntime));
	}
}
/* Handle a MERT configuration change */
cconfig() 
{

	if (!readrec(file)) {
		readerr++;
		return;
	}
	if(writout()) {
		need(7);
		printf(xlines[0],ctime(&e_hdr.e_time));
		printf(lines[0]);
		printf(xlines[1],dev[ercd.confchg.e_trudev],
			ercd.confchg.e_cflag?"Attached":"Detached");
	
		printf(lines[0]);
	}
}

/* Stray Interrupt Record */

stray()
{
	if (!readrec(file)) {
		readerr++;
		return;
	}
	if(!wanted()) return;
	if(print==YES) {
		need(7);
		if(page<=limit) {
		printf(xlines[2],ctime(&e_hdr.e_time));
		if(ercd.stray.e_saddr < DEVADR) printf(xlines[4]);
		else
		printf(xlines[3]);
		printf(FORM,ercd.stray.e_saddr);
		printf(lines[0]);
		printf(lines[7]);
		if(ercd.stray.e_sbacty == 0) printf("None\n");
		else
		afix(araysz(dev),(unsigned) ercd.stray.e_sbacty,dev);
		}
	}
	straysum++;
}


#define N -1  /* really no error, turned into "ecc turned off by kernel" */
#define	T -2  /* two errors, this should never get into the log */
#define M -3  /* three or more errors, should not appear either */
#define CX 16  /* check bits */
#define C0 17
#define C1 18
#define C2 19
#define C4 20
#define C8 21

/* the following table converts syndrome codes to bit in error
 * numbers.
 */

int stod[] = {
	 N, CX, C0,  T, C1,  T,  T,  M,
	C2,  T,  T,  1,  T,  M,  0,  T,
	C4,  T,  T,  2,  T,  3,  4,  T,
	 T,  5,  6,  T,  7,  T,  T,  M,
	C8,  T,  T,  8,  T,  9, 10,  T,
	 T, 11, 12,  T, 13,  T,  T,  M,
	 T, 14,  M,  T, 15,  T,  T,  M,
	 M,  T,  T,  M,  T,  M,  M,  N,
};

eccerr() {
	if (!readrec(file)) {
		readerr++;
		return;
		}
	if(!wanted()) return;
	if(print==YES) {
		need(7);
		if(page<=limit) {
			printf(xlines[5],ctime(&e_hdr.e_time));
			printf(xlines[7],ercd.ecc.e_bank);
			printf(lines[0]);
			printf(xlines[8],stod[0x3f & ercd.ecc.e_syndrome]);
			printf(lines[0]);
		}
	}
	eccsum++;
}

#ifdef PDP11
/* Memory Parity Record */

party()
{
	if (!readrec(file)) {
		readerr++;
		return;
		}
	if(!wanted())  return;
	if(print==YES) {
		need(9);
		if(page<=limit) {
		printf(xlines[5],ctime(&e_hdr.e_time));
		printf(xlines[6],(((long)(ercd.parity.e_parreg[1]&077))<<WDLEN) +
			((long)((unsigned) ercd.parity.e_parreg[0])),
			(ercd.parity.e_parreg[1]>>14)&03);
		printf(lines[8]);
		printf(xlines[7],ercd.parity.e_parreg[2]);
		printf(lines[0]);
		printf(xlines[8],ercd.parity.e_parreg[3]);
		printf(lines[0]);
		}
	}
	parsum++;
}
#endif
/* Device Error Record */

blk()
{
	register union ercd *z;
	register int i;
	struct regs *p;
	int *mbar;
	struct vaxreg *q;

	pos.flg = 0;
	if (!readrec(file)) {
		readerr++;
		return;
	}
	z = &ercd;
	MAJ=major(ercd.ebb.block.e_dev);
	MIN=minor(ercd.ebb.block.e_dev);
	if((MAJ>araysz(func)) | MAJ<0) return;
	if(!wanted()) return;
	(*func[MAJ])();
	/* Increment summary totals */
	
	if(ercd.ebb.block.e_bflags &E_ERROR)
	sums[MAJ][pos.unit].hard++;
	else
	sums[MAJ][pos.unit].soft++;
	sums[MAJ][pos.unit].totalio = ercd.ebb.block.e_stats.io_ops;
	sums[MAJ][pos.unit].misc = ercd.ebb.block.e_stats.io_misc;
	sums[MAJ][pos.unit].missing = ercd.ebb.block.e_stats.io_unlog;
	if(print==NO) return;
	need(pneed[MAJ]+ercd.ebb.block.e_nreg);
	if(page <= limit) {
		printf(lines[0]);
		printf(lines[1],dev[MAJ],ctime(&e_hdr.e_time));
		printf(lines[2],pos.unit);
		printf(lines[3],MIN,MIN);
		printf(lines[4]);
		printf(FORM,ercd.ebb.block.e_regloc);
		printf(lines[0]);
		printf(lines[5],ercd.ebb.block.e_rtry);
		printf(lines[6],ercd.ebb.block.e_bflags&E_ERROR?
			"Unrecovered":"Recovered");
		printf(lines[7]);
		if(ercd.ebb.block.e_bacty == 0) printf("None\n");
		else
		afix(araysz(dev),(unsigned) ercd.ebb.block.e_bacty,dev);
		printf(lines[0]);
		printf(lines[8]);
#ifdef vax
		mbar = ((int *) &z-> e_mba);
		q=mbareg;
		for(i=0;i<MBAREG;i++,mbar++,q++) {
			printf("\t  %-6.6s   %08X",q->regnam,*mbar);
			afix(WDLEN*2,*mbar,q->bitcod);
		}
#endif
		p=devregs[MAJ];
		for(i=0;i<ercd.ebb.block.e_nreg;i++,p++) {
			if(*p->regname != 'Z') {
				printf("\t  %-5.5s    ",p->regname);
				printf(FORM,ercd.ebb.reginf[i]);
				afix(WDLEN,ercd.ebb.reginf[i],p->bitcode);
			}
		}
		printf(lines[0]);
		printf(lines[10]);
		printf(FORM2,ercd.ebb.block.e_memadd);
		printf(lines[11],ercd.ebb.block.e_bytes);
		i=ercd.ebb.block.e_bflags;
		printf(lines[12],
		(i&E_NOIO)?"No-op":((i&E_READ)?"Read":"Write"));
		printf(lines[13],ercd.ebb.block.e_bnum);
		if(Unix) printf(lines[14],
			i&E_PHYS? "Physical":"Buffered");
		else line--;
		/* Not valid in this implementation; the line
		   following is inserted in its place.
		printf(lines[24],i&E_MAP?"Yes":"No");
		*/
		line--;
		printf(lines[0]);
		if(pos.flg) {
			printf(lines[0]);
			printf(lines[15],pos.cyl);
			printf(lines[16],pos.trk);
			printf(lines[17],pos.sector);
			printf(lines[0]);
			}
		printf(lines[0]);
		printf(lines[18]);
		printf(lines[19],ercd.ebb.block.e_stats.io_ops);
		printf(lines[20],ercd.ebb.block.e_stats.io_misc);
		printf(lines[21],ercd.ebb.block.e_stats.io_unlog);
		printf(lines[0]);
	}
}

dkblk() {
}

mtblk() {
}

pdblk() {
}

ptblk() {
}

isblk() {
}

rmblk() {
}

rkblk()
{
	register int m;
	daddr_t d;

	pos.flg = 1;
	m = MIN - 7;
	if(m<=0) {
		d = ercd.ebb.block.e_bnum;
		pos.unit = MIN;
	}
	else {
		d = (ercd.ebb.block.e_bnum/m);
		pos.unit =  (ercd.ebb.block.e_bnum%m);
		}
	pos.cyl = d/24;
	pos.trk = (d%24)/12;
	pos.sector = (d%24)%12;
}
rpblk()
{

	pos.flg=1;
	pos.unit = (MIN>>DSEC);
	pos.cyl=ercd.ebb.block.e_cyloff;
	pos.cyl += (ercd.ebb.block.e_bnum/(20*10));
	pos.trk = (ercd.ebb.block.e_bnum%(20*10))/10;
	pos.sector = ((ercd.ebb.block.e_bnum%(20*10))%10);
}

rfblk()
{
	pos.flg=1;
	pos.unit=(ercd.ebb.block.e_bnum/1024);
	pos.trk=((ercd.ebb.block.e_bnum%1024)/8);
	pos.cyl=0;
	pos.sector=((ercd.ebb.block.e_bnum%1024)%8);
}
tmblk()
{
	pos.unit = (MIN&03);
}
tcblk()
{
	pos.unit = (MIN&07);
}
hpblk()
{

	pos.flg = 1;
	pos.unit=(MIN>>DSEC);
	pos.cyl = ercd.ebb.block.e_cyloff;
	pos.cyl += (ercd.ebb.block.e_bnum/(19*22));
	pos.trk = ((ercd.ebb.block.e_bnum%(19*22))/22);
	pos.sector=((ercd.ebb.block.e_bnum%(19*22))%22);
}
htblk()
{
	pos.unit=(MIN&03);
}
hsblk()
{
	register div;

	if(MIN & 010) {
		div=32L;
		n=1;
		}
		else {
		div=16L;
		n=3;
		}
	pos.flg = 1;
	pos.cyl=0;
	pos.unit=MIN&07;
	pos.trk = (ercd.ebb.block.e_bnum/div);
	pos.sector = ercd.ebb.block.e_bnum%div;
	pos.sector *= (n+1);
}
rlblk()
{
	pos.flg = 1;
	pos.unit = MIN;
	pos.trk = (ercd.ebb.block.e_bnum/(20*256));
	pos.cyl = ((ercd.ebb.block.e_bnum)%(20*256))/20;
	pos.sector = ((ercd.ebb.block.e_bnum)%(20*256))%20;
	pos.sector *= 2;
}
cleanse(p,q)
	register char *p;
	register int q;
{
	while(q--)
		*p++='\0';
}

afix(a,b,c)
int a;
unsigned b;
char **c;
{
	register i;
	cleanse(interp,MAXSTR);
	for(i=0;i<a;i++)  {
		if((b & (1<<i)) && (*c[i]))    {
			if((strlen(c[i])+strlen(interp))>=MAXSTR) {
				concat(",",interp);
				printf("          %s\n\t\t\t",interp);
				line++;
				cleanse(interp,MAXSTR);
			}
			else {
				if(strlen(interp)) concat(",",interp);
			}
			concat(c[i],interp);
		}
	}
	printf("      %s\n",interp);
}
puthead(h)
char *h;
{
	printf("\n\n   %s   Prepared on %s     Page  %d\n\n\n\n",
		h,htime,page);
	line = 6;
}
inithdng()
{

	time(&tloc);
	ctime(&tloc);
	cbuf[16] = '\0';
	strcpy(htime,cbuf+4);
}
putft()
{
	while (line++<MAXLEN) {
		putchar('\n');
	}
	page++;
}
trnpg()
{
	if( line >= MAXLEN) page++;
	else putft();
	if(page<=limit) puthead(header);
}
need(a)			/* acts like ".ne" command of nroff */
int a;
{
	if( line>(PGLEN-a)) trnpg();
	line += a;
}
gtime(tptr,pt)
char *pt;
time_t	*tptr;
{
	register int i;
	register int y, t;
	int d, h, m;
	extern	int	dmsize [];
	extern int *localtime();
	extern long timezone;
	extern long timezone;
	int nt[2];

	t = gpair(pt++);
	if(t<1 || t>12)
		return(1);
	pt++;
	d = gpair(pt++);
	if(d<1 || d>31)
		return (1);
	pt++;
	h = gpair(pt++);
	if(h == 24) {
		h = 0;
		d++;
	}
	pt++;
	m = gpair(pt++);
	if(m<0 || m>59)
		return (1);
	pt++;
	y = gpair(pt++);
	if (y<0) {
		time(nt);
		y = localtime(nt)[5];
	}
	*tptr = 0;
	y += 1900;
	for(i=1970; i<y; i++)
		*tptr += dysize(i);
	/* Leap year */
	if (dysize(y)==366 && t >= 3)
		*tptr += 1;
	while(--t)
		*tptr += dmsize[t-1];
	*tptr += (d-1);
	*tptr = (*tptr *24) + h;
	*tptr = (*tptr*60) + m;
	*tptr *= 60;
	*tptr += timezone;
	if(localtime(tptr)->tm_isdst)
		*tptr -= 60*60;
	return(0);

}
gpair(pt)
char *pt;
{
	register int c, d;
	register char *cp;

	cp = pt;
	if(*cp == 0)
		return(-1);
	c = (*cp++ - '0') * 10;
	if (c<0 || c>100)
		return(-1);
	if(*cp == 0)
		return(-1);
	if ((d = *cp++ - '0') < 0 || d > 9)
		return(-1);
	return (c+d);
}

wanted ()
{
	/* Starting - ending limitations? */
	if(e_hdr.e_time <stime ) return (0);
	if(e_hdr.e_time > etime) return (0);
	/* Only fatal error flag? */
	if((fflg) && (e_hdr.e_type==E_BLK) && !(ercd.ebb.block.e_bflags&E_ERROR)) return(0);
	/* Stray interrupts or ecc errors to be considered */
	if((aflg) || ((e_hdr.e_type==E_STRAY)&&(optdev&(1<<INT))) ||
		((e_hdr.e_type==E_ECC)&&(optdev&(1<<MEM)))) {
		print=YES;
		return (1);
		}
	/* Device chosen for consideration or printing? */
	if(dflg == 0) {
		print=NO;
		return(1);
		}
	if((1<<MAJ)&optdev) {
		print=YES;
		return(1);
		}
	print=NO;
	return(0);
}
error(s1,s2)
char *s1, *s2;
{
	fprintf(stderr,"errpt:%s %s \n",s1,s2);
	exit(16);
}


recov()
{
	struct errhdr *p,*q;
	int i;
	for(;;) {
		p = q = &e_hdr;
		q++;
		for(i=0;i<((sizeof(struct errhdr) /2)-1);i++)
			*p++ = *q++;
		fread(p,2,1,file);
		if(feof(file))return(0);
		if(valid()) return (1);
	}
}
valid()
{
	switch(e_hdr.e_type) {
		default:
			return(0);
		case E_GOTS:
		case E_GORT:
		case E_STOP:
		case E_TCHG:
		case E_BLK:
		case E_STRAY:
		case E_CCHG:
		case E_PRTY:
		case E_ECC:
	if((e_hdr.e_len <MINREC) ||
		 (e_hdr.e_len > MAXREC) ) return (0);
	if((e_hdr.e_time < atime) ||
		 (e_hdr.e_time > tloc))  return(0);
	return (1);
}
}
printsum()
{
	int i;
	header = hd2;
	page = 1;
	puthead(header);
	need(11);
	printf(sumlines[12],choice[0]?choice:"All");
	printf(sumlines[13]);
	if(stime) {
		printf("On or after %s",ctime(&stime));
		printf(sumlines[14]);
	}
	else line--;
	if(etime!=017777777777L) {
		printf("On or before %s",ctime(&etime));
		printf(sumlines[14]);
	}
	else line--;
	if(fflg) {
		printf("Only fatal errors are printed.\n");
		printf(sumlines[14]);
	}
	else line--;
	if(limit != 10000) printf("Printing suppressed after page %d.\n",limit);
	else line--;
	printf(lines[0]);
	printf(sumlines[10],ctime(&fftime));
	printf(sumlines[11],ctime(&ltime));
	printf(lines[0]);
	if(readerr) printf(sumlines[7],readerr);
	else printf(lines[0]);
	printf(lines[0]);
	if((optdev&(1<<14)) || !(dflg) || (aflg)) {
		need(3);
		printf(lines[0]);
		printf(sumlines[9],straysum);
		printf(lines[0]);
		}
	if((optdev&(1<<13)) || !(dflg) || (aflg)) {
		need(3);
		printf(lines[0]);
		printf(sumlines[8],eccsum);
		printf(lines[0]);
		}
	if ((dflg == 0)||(aflg)) {
		for(i=0;i<NMAJOR;i++) (prsum(i)); }
	else
	for(i=0;i<NMAJOR;i++)
		if(optdev & (1<<i)) prsum(i);
	if(line == 7) printf("No errors for this report\n");
}

prsum(i)
register int i;
{
	register int j;

	for(j=0;j<6;j++) {
		if(sums[i][j].totalio) {
			need(10);
			printf(sumlines[1],dev[i],j);
			printf(sumlines[0]);
			printf(sumlines[2],sums[i][j].hard);
			printf(sumlines[3],sums[i][j].soft);
			printf(sumlines[4],sums[i][j].totalio);
			printf(sumlines[5],sums[i][j].misc);
			printf(sumlines[6],sums[i][j].missing);
			printf(sumlines[0]);
		}
	}
}
/* Associate typed name with a specific bit in "optdev" */
encode(p)
char  *p;
{
	register struct tab *q;
	for(q=dtab;q->devname;q++) {
		if (!strcmp(q->devname,p))
			return(1<<(q->devnum));
	}
	return(0);
}

concat(a,b)
	register char *a,*b;
{
	while (*b) b++;
	while (*b++ = *a++);
}
setime()
{
	if(e_hdr.e_time < fftime)
	fftime = e_hdr.e_time;
	if(e_hdr.e_time > ltime)
	ltime = e_hdr.e_time;
}
