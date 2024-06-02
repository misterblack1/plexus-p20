/*
 * This file contains code that handles the formatting of disks.
 */

#include	"sys/is.h"
#include	"sys/plexus.h"

/*
#define	dbunit	((struct db_i *) ((WUA << 4) - 0x4b0))
#define	dbzero	((struct db_i *) ((WUA << 4) - 0x4a8))
#define	dbdata	((struct db_f *) ((WUA << 4) - 0x4a0))
#define dbdef	((struct db_f *) ((WUA << 4) - 0x498))
#define	dbalt	((struct db_f *) ((WUA << 4) - 0x490))
 */
#define	D_CYLS	823			/* default number of cylinders */
#define	D_RHEADS	0		/* default number of removable heads */
#define	D_FHEADS	4		/* default number of fixed heads */
#define D_BPS	512			/* default bytes per sector */
#define	D_SPT	35			/* default num. of sectors per track */
#define	D_ALTCYLS	25		/* default number of alternate cyls. */
#define D_UP1	0xa5			/* part of default user pattern */
#define D_UP2	0xa5			/* part of default user pattern */
#define D_INTERL	4		/* default interleave factor */
#define	D_DTYPE	0			/* format data track type */
#define D_DFTYPE	0x80		/* format defective track type */
#define D_ATYPE		0x40		/* format alternate track type */
#define	DLMAX	250			/* defective tracks list maxi. entries */
#define	ERRFLAGS	0xc0		/* error flag in cib.c_opstat */
#define	DF_FORMAT	0x2		/* format function value */
char	spstate, p_both, p_unit, afhead, arhead;
int	afcyl, arcyl;
unsigned	p_cylinders, p_sector;
char	p_userp[8];
struct	dlist {
	unsigned	dl_cylinder;
	char	dl_head;
} dl_f[DLMAX], dl_r[DLMAX];
long	tocnt;						/* timeout counter */
struct	db_i {
	unsigned	db_cyls;
	char	db_rheads;
	char	db_fheads;
	char	db_bpsl;
	char	db_spt;
	char	db_altcyls;
	char	db_bpsh;
};

struct	db_i	dbunit;
struct	db_i dbzero = { 0, 0, 0, 0, 0, 0, 0 };
struct	db_f {
	char	dbf_up1;
	char	dbf_type;
	char	dbf_up2;
	char	dbf_up3;
	char	dbf_intl;
	char	dbf_up4;
};
struct	db_f	dbdata;
struct	db_f dbdef;
struct	db_f dbalt;
struct	alist {
	char	al_used;
	char	al_head;
	unsigned	al_cyl;
} al_f[DLMAX], al_r[DLMAX];
char	buf[512];
char	initbuf[1024];
char	wbuf[10];
char	debug;
			
/*
 * mywaitgo - prints message 'Type <cr> to go.' and waits for input from
 *            operator.  Enter anything to continue program execution.
 *            Typing 'd' causes debug to be turned off.
 */

mywaitgo()
{

	printf( "Type <cr> to go. " );
	gets( wbuf );
	if ( wbuf[0] == 'd' ) {
		debug = 0;
	}
}

/*
 * diagtrk - format the diagnostic track(s) for this drive.  The iSBC220
 *           requires that the track under head 0 on the last cylinder
 *           of each drive must be formatted as a data track for the 
 *           controller diagnostics.
 */

diagtrk()
{

	iop.i_cylinder = (dbunit.db_cyls - 1 );
	iop.i_head = 0;
	iop.i_dboff = (unsigned)&dbdata;
	cib.c_statsem = 0;
	if ( dbunit.db_fheads > 0 ) {
		iop.i_unit = p_unit;
		iop.i_function = DF_FORMAT;
		debuginfo();
		if (fwaitdisk()) {
			printf( "Can't format fixed diagnostic track, " );
			printf( "%d, %d, %d\n",
				iop.i_unit, iop.i_cylinder, iop.i_head );
			exit(1);
		}
		if ( debug )
			printf( "Fixed diagnostic track formatted.\n" );
	}
	if ( dbunit.db_rheads > 0 ) {
		iop.i_unit = ( p_unit | ( ( REMOVABLE << 4 ) & p_both ) );
		iop.i_function = DF_FORMAT;
		debuginfo();
		if (fwaitdisk()) {
			printf( "Can't format removable diagnostic track, " );
			printf( "%d, %d, %d\n",
				iop.i_unit, iop.i_cylinder, iop.i_head );
			exit(1);
		}
		if ( debug )
			printf( "Removable diagnostic track formatted.\n" );
	}
}

/*
 * alttrk - formats the 'alternates track' used by dformat to hold the 
 *          current status of all the alternate tracks on a drive.
 */

alttrk()
{
	int	i, j, k;

	iop.i_cylinder = (dbunit.db_cyls - 1 );
	iop.i_head = 1;
	iop.i_dboff = (unsigned)&dbdata;
	cib.c_statsem = 0;
	if ( dbunit.db_fheads > 0 ) {
		if ( dbunit.db_fheads == 1 ) {
			iop.i_head = 0;
			iop.i_cylinder--;
			dbunit.db_cyls--;
			dbunit.db_altcyls--;
		}
		iop.i_unit = p_unit;
		iop.i_function = DF_FORMAT;
		afcyl = iop.i_cylinder;
		afhead = iop.i_head;
		if ( debug ) {
		debuginfo();
		}
		if (fwaitdisk()) {
			printf( "Can't format fixed alternates track, " );
			printf( "%d, %d, %d\n",
				iop.i_unit, iop.i_cylinder, iop.i_head );
			exit(1);
		}
		if ( debug )
			printf( "Fixed alternates track formatted.\n" );
		iop.i_cylinder = dbunit.db_cyls - dbunit.db_altcyls;
		for ( i = 0, k = 0; i < ( dbunit.db_altcyls - 1 ); i++ ) {
			for ( j = 0; j < dbunit.db_fheads; j++, k++ ) {
				al_f[k].al_used = 0;
				al_f[k].al_head = j;
				al_f[k].al_cyl = iop.i_cylinder;
			}
			iop.i_cylinder++;
		}
		for ( k = k; k < DLMAX; k++ )
			al_f[k].al_used = 0xff;
	} else {
		for ( k = 0; k < DLMAX; k++ )
			al_f[k].al_used = 0xff;
	}
	if ( dbunit.db_rheads > 0 ) {
		if ( dbunit.db_rheads == 1 ) {
			iop.i_head = 0;
			iop.i_cylinder--;
			dbunit.db_cyls--;
			dbunit.db_altcyls--;
		}
		iop.i_unit = ( p_unit | ( ( REMOVABLE << 4 ) & p_both ) );
		iop.i_function = DF_FORMAT;
		arcyl = iop.i_cylinder;
		arhead = iop.i_head;
		debuginfo();
		if (fwaitdisk()) {
			printf( "Can't format removable alternates track, " );
			printf( "%d, %d, %d\n",
				iop.i_unit, iop.i_cylinder, iop.i_head );
			exit(1);
		}
		if ( debug )
			printf( "Removable alternates track formatted.\n" );
		iop.i_cylinder = dbunit.db_cyls - dbunit.db_altcyls;
		for ( i = 0, k = 0; i < ( dbunit.db_altcyls - 1 ); i++ ) {
			for ( j = 0; j < dbunit.db_rheads; j++, k++ ) {
				al_r[k].al_used = 0;
				al_r[k].al_head = j;
				al_r[k].al_cyl = iop.i_cylinder;
			}
			iop.i_cylinder++;
		}
		for ( k = k; k < DLMAX; k++ )
			al_r[k].al_used = 0xff;
	} else {
		for ( k = 0; k < DLMAX; k++ )
			al_r[k].al_used = 0xff;
	}
#ifdef DEBUG
		if ( debug ) {
			{	int i,j;
				int *iptr;
				iptr = (int *)al_f;
				printf( "FIXED ALTERNATES LIST\n" );
				for (i=0; i<(DLMAX/5); i++ ) {
					for ( j = 0; j < 10; j++ ) {
						printf("%x, ",*iptr++);
					}
				printf("\n");
				}
				mywaitgo();
				iptr = (int *)al_r;
				printf( "REMOVABLE ALTERNATES LIST\n" );
				for (i=0; i<(DLMAX/5); i++) {
					for ( j=0; j<10; j++ ) {
						printf("%x, ",*iptr++);
					}
				printf( "\n" );
				}
			
			}
		mywaitgo();
		}
#endif
}

/*
 * rdalt - reads the 'alternates track' used by dformat to determine
 *         availability of alternate tracks on a drive.
 */

rdalt()
{
	int	*p, *q, *r, i, temp;
	int	falts, fcyls, ralts, rcyls;

	iop.i_function = DF_READ;
	if ( dbunit.db_fheads > 0 ) {
		iop.i_unit = p_unit;
		iop.i_cylinder = (dbunit.db_cyls - 1);
		iop.i_head = 1;
		fcyls = dbunit.db_cyls;
		falts = dbunit.db_altcyls;
		if ( dbunit.db_fheads == 1 ) {
			iop.i_head = 0;
			iop.i_cylinder--;
			fcyls--;
			falts--;
		}
		iop.i_sector = 0;
		iop.i_dboff = (unsigned)buf;
		iop.i_rc1 = 0x200;
		iop.i_rc2 = 0;
		cib.c_statsem = 0;
		afcyl = iop.i_cylinder;
		afhead = iop.i_head;
		debuginfo();
		if (fwaitdisk()) {
			printf( "Can't read fixed alternates sector, " );
			printf( "%d, %d, %d\n",
				iop.i_unit, iop.i_cylinder, iop.i_head );
			exit(1);
		}
		if ( debug )
			printf( "Fixed alternates sector read.\n" );
		p = (int *)buf;
		q = (int *)&dbunit;
		for ( i = 0; i < 4; i++ ) {
			if ( debug ) {
				printf( "p = %x, q = %x\n", p, q );
				printf( "*p = %x, *q = %x\n", *p, *q );
				mywaitgo();
			}
			if ( *p++ != *q++ ) {
				printf( "Specified parameters do not match " );
				printf( "those on disk.\n" );
				printf( "Parameters on disk are: \n" );
				temp = (buf[0] << 8) | buf[1];
				printf("cyls = %d, rheads = %d, fheads = %d",
					temp, buf[2], buf[3] );
				printf( ", bpsl = %x, spt = %d, ",
					buf[4], buf[5] );
				printf( "interl = %d, bpsh = %x\n",
					buf[6], buf[7] );
				exit(1);
			}
		}
		q = (int *)&buf[8];
		p = (int *)al_f;
		for ( i = 0; i < (DLMAX/2); i++ ) {
			*p++ = *q++;
		}
	}
	if ( dbunit.db_rheads > 0 ) {
		iop.i_unit = ( p_unit | (( REMOVABLE << 4 ) & p_both ));
		iop.i_cylinder = (dbunit.db_cyls - 1);
		iop.i_head = 1;
		rcyls = dbunit.db_cyls;
		ralts = dbunit.db_altcyls;
		if ( dbunit.db_rheads == 1 ) {
			iop.i_head = 0;
			iop.i_cylinder--;
			rcyls--;
			ralts--;
		}
		iop.i_sector = 0;
		iop.i_dboff = (unsigned)buf;
		iop.i_rc1 = 0x200;
		iop.i_rc2 = 0;
		arcyl = iop.i_cylinder;
		arhead = iop.i_head;
		debuginfo();
		if (fwaitdisk()) {
			printf( "Can't read removable alternates sector, " );
			printf( "%d, %d, %d\n",
				iop.i_unit, iop.i_cylinder, iop.i_head );
			exit(1);
		}
		if ( debug )
			printf( "Removable alternates sector read.\n" );
		p = (int *)buf;
		q = (int *)&dbunit;
		for ( i = 0; i < 4; i++ ) {
			if ( *p++ != *q++ ) {
				printf( "Specified parameters do not match " );
				printf( "those on disk.\n" );
				printf( "Parameters on disk are: \n" );
				temp = (buf[0] << 8) | buf[1];
				printf("cyls = %d, rheads = %d, fheads = %d", 
					temp, buf[2], buf[3] );
				printf( ", bpsl = %x, spt = %d, ",
					buf[4], buf[5] );
				printf( "interl = %d, bpsh = %x\n",
					buf[6], buf[7] );
				exit(1);
			}
		}
		q = (int *)&buf[8];
		p = (int *)al_r;
		for ( i = 0; i < (DLMAX/2); i++ ) {
			*p++ = *q++;
		}
	}
#ifdef DEBUG
	if ( debug ) {
		{	int i,j;
			int *iptr;
			iptr = (int *)al_f;
			printf( "FIXED ALTERNATES LIST\n" );
			for (i=0; i<(DLMAX/5); i++ ) {
				for ( j = 0; j < 10; j++ ) {
					printf("%x, ",*iptr++);
				}
			printf("\n");
			}
			mywaitgo();
			iptr = (int *)al_r;
			printf( "REMOVABLE ALTERNATES LIST\n" );
			for (i=0; i<(DLMAX/5); i++) {
				for ( j=0; j<10; j++ ) {
					printf("%x, ",*iptr++);
				}
			printf( "\n" );
			}
		
		}
	mywaitgo();
	}
#endif
}
/*
 * wrtalt - writes the 'alternates track' used by dformat to determine
 *          availability of alternate tracks on each drive.
 */

wrtalt()
{
	int	*p, *q, *r, i;
	int	fcyls, falts;
	iop.i_function = DF_WRITE;
	p = (int *)buf;
	q = (int *)&dbunit;
	for ( i = 0; i < 4; i++ ) {
		*p++ = *q++;
	}
	r = p;
	if ( dbunit.db_fheads > 0 ) {
		q = (int *)al_f;
		for ( i = 0; i <(DLMAX/2); i++ ) {
			*p++ = *q++;
		}
		iop.i_cylinder = afcyl;
		iop.i_head = afhead;
		iop.i_dboff = (unsigned)buf;
		iop.i_rc1 = 0x0200;
		iop.i_rc2 = 0;
		iop.i_sector = 0;
		cib.c_statsem = 0;
		if (fwaitdisk()) {
			printf( "Can't write fixed alternates sector\n" );
			printf( "cyl = %x, head = %x, sector = %x\n",
				iop.i_cylinder, iop.i_head, iop.i_sector );
			derror();
			exit(1);
		}
		if ( debug ) {
			{	int	*iptr;
				iptr = (int *)&cib;
				iptr++;
				printf( "cib+2 = %x\n", *iptr );
			}
		mywaitgo();
		}
	}
	if ( dbunit.db_rheads > 0 ) {
		p = r;
		q = (int *)al_r;
		for ( i = 0; i <(DLMAX/2); i++ ) {
			*p++ = *q++;
		}
		iop.i_cylinder = arcyl;
		iop.i_head = arhead;
		iop.i_sector = 0;
		iop.i_dboff = (unsigned)buf;
		iop.i_rc1 = 0x200;
		iop.i_rc2 = 0;
		cib.c_statsem = 0;
		if (fwaitdisk()) {
			printf( "Can't write removable alternates sector\n" );
			printf( "cyl = %x, head = %x, sector = %x\n",
				iop.i_cylinder, iop.i_head, iop.i_sector );
			derror();
			exit(1);
		}
	}
}

/*
 * dinit - initializes the iSBC220 disk controller for the formatting or
 *         sparing of a particular drive.
 */

dinit()
{

	int	i;
	WUB->w_wubr1 = 0;
	WUB->w_soc = 1;
	WUB->w_ccboff = (unsigned) &ccb;
	WUB->w_ccbseg = xmemto8086seg( MBSDSEG );
	ccb.c_bsy1 = 0xff;
	out_multibus( WUA, OP_RESET );
	out_multibus( WUA, OP_CLEAR );
	out_multibus( WUA, OP_START );
	for ( tocnt = MAXTOCNT; tocnt && ccb.c_bsy1; tocnt-- ) {
	}
	if ( !tocnt ) {
		printf( "Disk controller doesn't wake-up - dformat aborted\n" );
		exit(1);
	}
	if ( debug )
		printf( "Controller wakeup OK\n" );
	ccb.c_ccw1 = 1;
	ccb.c_ciboff = (unsigned) &cib.c_csaoff;
	ccb.c_cibseg = xmemto8086seg( MBSDSEG );
	ccb.c_ccbr1 = 0;
	ccb.c_bsy2 = 0;
	ccb.c_ccw2 = 1;
	ccb.c_cpoff = (unsigned)&ccb.c_cp;
	ccb.c_cpseg = xmemto8086seg( MBSDSEG );
	ccb.c_cp = 4;

	cib.c_opstat = 0;
	cib.c_cibr1 = 0;
	cib.c_statsem = 0;
	cib.c_cmdsem = 0;
	cib.c_csaoff = 0;
	cib.c_csaseg = 0;
	cib.c_iopoff = (unsigned) &iop;
	cib.c_iopseg = xmemto8086seg( MBSDSEG );
	cib.c_cibr2 = 0;
	cib.c_cibr3 = 0;

	iop.i_iopr1 = 0;
	iop.i_iopr2 = 0;
	iop.i_ac1 = 0;
	iop.i_ac2 = 0;
	iop.i_iopr3 = 0;
	iop.i_function = DF_INIT;
	iop.i_modifier = 1;
	iop.i_cylinder = 0;
	iop.i_sector = 0;
	iop.i_head = 0;
	iop.i_dbseg = xmemto8086seg( MBSDSEG );
	iop.i_rc1 = 0;
	iop.i_rc2 = 0;
	iop.i_iopr4 = 0;
	iop.i_iopr5 = 0;
	for ( i = 0; i < 4; i++ ) {
		if ( i == p_unit )
			iop.i_dboff = (unsigned) &dbunit;
		else iop.i_dboff = (unsigned) &dbzero;
		dbzero.db_cyls = dbzero.db_rheads = dbzero.db_fheads = 0;
		dbzero.db_bpsl = dbzero.db_spt = dbzero.db_altcyls = 0;
		dbzero.db_bpsh = 0;
		iop.i_unit = i;
		cib.c_statsem = 0;
		cib.c_opstat = 0;
		iop.i_ac1 = iop.i_ac2 = 0;
		debuginfo();
		if (fwaitdisk()) {
			printf( "Can't initialize unit = %d\n", iop.i_unit );
			derror();
			exit(1);
		}
		if ( debug )
			printf( "Unit = %d initialized\n", iop.i_unit );
	}
}

/*
 * hexchk - checks input buffer for hexdecimal input validity.
 */

hexchk( hex )
char	*hex;
{

	int	i, j, gotonext, digit;

	j = gotonext = 0;
	hex[j] = 0;
	for ( i = 0; (i < 8) && ( buf[i] != '\0' ); i++ ) {
		if ( (( buf[i] - '0' ) >= 0) && (( buf[i] - '0' ) <= 9) )
			digit = ( buf[i] - '0' );
		else if ( ( buf[i] - 'a' ) <= 6 && ( buf[i] - 'a' ) >= 0 )
			digit = (buf[i] - 'a') + 10;
		else return(0);
		if ( gotonext ) {
			hex[j] |= digit;
			gotonext--;
			j++;
			hex[j] = 0;
		} else {
			hex[j] = (( hex[j] |= digit ) << 4);
			gotonext++;
		}
	}
	if ( i != 8 )
		return(0);
	else return( 1 );
}

/*
 * numchk - checks input buffer for numeric input within a specified 
 *          range.
 */

numchk( buf, l, h )
char	*buf;
int l, h;
{

	int	num;

	num = atol( buf );
	if ( ( num >= l ) && ( num <= h ) )
		return( num );
	else return( -1 );
}

/*
 * setdef - sets default parameters and prompts for any changes to the
 *          defaults.
 */

setdef()
{

	int	i, j, k, temp;
	char	tchar[32];

	if (getinitinfo()) {
		for (;;) {
			printf( "User pattern? [8 hex digits]: " );
			gets( buf );
			if ( buf[0] == '\0' )
				break;
			temp = hexchk( tchar );
			if ( !temp ) {
				printf( "\n" );
				continue;
			} else {
				dbdata.dbf_up1 = tchar[0];
				dbdata.dbf_up2 = tchar[1];
				dbdata.dbf_up3 = tchar[2];
				dbdata.dbf_up4 = tchar[3];
				break;
			}
		}
	}
	for ( i = 0; (i < DLMAX) && dbunit.db_fheads; ) {
		if ( spstate )
			break;
		else printf( "Defective tracks on fixed? " );
		printf( "(cyl,head) [0-%d,0-%d]: ", (dbunit.db_cyls - 1 ),
			 (dbunit.db_fheads - 1) );
		gets( buf );
		if ( buf[0] == '\0' )
			break;
		for ( j = 0; j < 5; j++ ) {
			tchar[j] = buf[j];
			if ( tchar[j] == ',' ) {
				tchar[j] = '\0';
				break;
			}
			if ( tchar[j] == '\0' ) {
				j = 5;
				break;
			}
		}
		if ( j < 5 ) {
			temp = numchk( tchar, 0, (dbunit.db_cyls - 1) );
			if ( temp == (-1) ) { 
				printf( "Cylinder out of range.\n" );
				continue;
			} else {
				dl_f[i].dl_cylinder = temp;
			}
		} else {
			printf( "Cylinder out of range.\n" );
			continue;
		}
		j++;
		for ( k = 0; k < 2; k++, j++ ) {
			tchar[k] = buf[j];
			if ( tchar[k] == '\0' )
				break;
		}
		if ( k < 2 ) {
			temp = numchk( tchar, 0, (dbunit.db_fheads - 1) );
			if ( temp == (-1) ) {
				printf( "Head out of range.\n" );
				continue;
			}
			dl_f[i].dl_head = temp;
			if ( i++ > ((dbunit.db_cyls - 1) * dbunit.db_fheads) || i == DLMAX ) {
				printf( "\nToo many defective tracks!\n" );
				break;
			} else continue;
		} else {
			printf( "Head out of range.\n" );
			continue;
		}
	}
	if ( i < DLMAX )
		dl_f[i].dl_head = 0xff;
	for ( i = 0; (i < DLMAX) && dbunit.db_rheads; ) {
		if ( spstate )
			break;
		else printf( "Defective tracks on removable? " );
		printf( "(cyl,head) [0-%d,0-%d]: ", (dbunit.db_cyls - 1),
			 (dbunit.db_rheads - 1) );
		gets( buf );
		if ( buf[0] == '\0' )
			break;
		for ( j = 0; j < 5; j++ ) {
			tchar[j] = buf[j];
			if ( tchar[j] == ',' ) {
				tchar[j] = '\0';
				break;
			}
			if ( tchar[j] == '\0' ) {
				j = 5;
				break;
			}
		}
		if ( j < 5 ) {
			temp = numchk( tchar, 0, (dbunit.db_cyls - 1) );
			if ( temp == (-1) ) { 
				printf( "Cylinder out of range.\n" );
				continue;
			} else {
				dl_f[i].dl_cylinder = temp;
			}
		} else {
			printf( "Cylinder out of range.\n" );
			continue;
		}
		j++;
		for ( k = 0; k < 2; k++, j++ ) {
			tchar[k] = buf[j];
			if ( tchar[k] == '\0' )
				break;
		}
		if ( k < 2 ) {
			temp = numchk( tchar, 0, (dbunit.db_fheads - 1) );
			if ( temp == (-1) ) {
				printf( "Head out of range.\n" );
				continue;
			}
			dl_f[i].dl_head = temp;
			if ( i++ > ((dbunit.db_cyls - 1) * dbunit.db_rheads) || i == DLMAX ) {
				printf( "\nToo many defective tracks!\n" );
				break;
			} else continue;
		} else {
			printf( "Head out of range.\n" );
			continue;
		}
	}
	if ( i < DLMAX )
		dl_r[i].dl_head = 0xff;
#ifdef DEBUG
	if ( debug  ) {
		{	int	i,j;
			int	*iptr;
			iptr = (int *)dl_f;
			printf( "FIXED DEFECTIVES LIST\n" );
			for ( i=0; i<DLMAX/5; i++ ) {
				for ( j=0; j<10; j++ ) {
					printf( "%x, ", *iptr++ );
				}
			printf( "\n" );
			}
			mywaitgo();
			iptr = (int *)dl_r;
			printf( "REMOVABLE DEFECTIVES LIST\n" );
			for ( i=0; i<DLMAX/5; i++ ) {
				for ( j=0; j<10; j++ ) {
					printf( "%x, ", *iptr++ );
				}
			printf( "\n" );
			}
		}
	mywaitgo();
	}
#endif
}

reinit() {

	getinitinfo();
	getfsinfo();
	dinit();
	writezero();

	printf("Disk initialized successfully!\n");
}

getfsinfo() {
	int i;
	int temp;

	for (i=0; i<sizeof initbuf; i++)
		initbuf[i] = 0;

	for (i=0; i<4; i++)
		((int *)initbuf)[i] = ((int *)&dbunit)[i];

	for (;;) {
		printf("File system blocksize? [512-8192]: ");
		gets(buf);
		if ( buf[0] == '\0')
			continue;
		temp = numchk( buf, 512, 8192 );
		if ( temp == (-1) ) {
			printf( "\n" );
			continue;
		} else {
			((int *)initbuf)[4] = temp;
			((int *)initbuf)[5] = 'is';
			break;
		}
	}
	for (;;) {
		printf("Default boot name? : ");
		gets(&initbuf[16]);
		if ( initbuf[16] != '\0')
			break;
	}
}


writezero() {
	iop.i_iopr1 = 0;
	iop.i_iopr2 = 0;
	iop.i_iopr3 = 0;
	iop.i_function = DF_WRITE;
	iop.i_unit = p_unit;
	iop.i_modifier = MOD_NOINT;
	iop.i_cylinder = 0;
	iop.i_sector = 0;
	iop.i_head = 0;
	iop.i_head = 0;
	iop.i_dboff = (unsigned) initbuf;
	iop.i_dbseg = xmemto8086seg( MBSDSEG );
	iop.i_rc1 = sizeof(initbuf);
	iop.i_rc2 = 0;
	iop.i_iopr4 = 0;
	iop.i_iopr5 = 0;
	cib.c_statsem = 0;
	debuginfo();
	if (fwaitdisk()) {
		printf("Could not initialize block 0 of disk drive %d\n",p_unit);
		exit(1);
	}
}

getinitinfo() {
	int i, temp;

	/* Set up initialization data block with defaults. */

	dbunit.db_cyls = D_CYLS;
	dbunit.db_rheads = D_RHEADS;
	dbunit.db_fheads = D_FHEADS;
	dbunit.db_bpsl =  ( D_BPS & 0xff );
	dbunit.db_spt = D_SPT;
	dbunit.db_altcyls = D_ALTCYLS;
	dbunit.db_bpsh =  ( ( D_BPS >> 8 ) & 0xff );

	/* Set up format data block defaults. */

	dbdata.dbf_up1 = dbdef.dbf_up1 = dbalt.dbf_up1 = D_UP1;
	dbdata.dbf_type = D_DTYPE;
	dbdef.dbf_type = D_DFTYPE;
	dbalt.dbf_type = D_ATYPE;
	dbdata.dbf_up2 = dbdef.dbf_up2 = dbalt.dbf_up2 = D_UP2;
	dbdata.dbf_up3 = dbdef.dbf_up3 = dbalt.dbf_up3 = D_UP1;
	dbdata.dbf_intl = dbdef.dbf_intl = dbalt.dbf_intl = D_INTERL;
	dbdata.dbf_up4 = dbdef.dbf_up4 = dbalt.dbf_up4 = D_UP2;

	for (;;) {
		printf( "Total number of cylinders? [3-1024]: " );
		gets( buf );
		if ( buf[0] == '\0' )
			continue;
		if (atol(buf) == -1) {
			if ((dbunit.db_fheads == 0) || (dbunit.db_rheads == 0))
				p_both = 0;
			else p_both = ( REMOVABLE << 4);
			dl_f[0].dl_head = 0xff;
			dl_r[0].dl_head = 0xff;
			return(0);
		}
		temp = numchk( buf, 3, 1024 );
		if ( temp == (-1) ) {
			printf( "\n" );
			continue;
		} else {
			dbunit.db_cyls = temp;
			break;
		}
	}
	for (;;) {
		printf( "Number of heads on removable? [0-24]: " );
		gets( buf );
		if ( buf[0] == '\0' ) 
			continue;
		temp = numchk( buf, 0, 24 );
		if ( temp == (-1) ) {
			printf( "\n" );
			continue;
		} else {
			dbunit.db_rheads = temp;
			break;
		}
	}
	for (;;) {
		printf( "Number of heads on fixed? [0-24]: " );
		gets( buf );
		if ( buf[0] == '\0' ) 
			continue;
		temp = numchk( buf, 0, 24 );
		if ( temp == (-1) ) {
			printf( "\n" );
			continue;
		} else {
			dbunit.db_fheads = temp;
			break;
		}
	}
	if ( dbunit.db_fheads == 0 || dbunit.db_rheads == 0 )
		p_both = 0;
	else p_both = ( REMOVABLE << 4 );
	for (;;) {
		printf( "Data bytes per sector? [32-1024]: " );
		gets( buf );
		if ( buf[0] == '\0' )
			continue;
		temp = numchk( buf, 32, 1024 );
		if ( temp == (-1) ) {
			printf( "\n" );
			continue;
		} else {
			dbunit.db_bpsl = ( temp & 0xff );
			dbunit.db_bpsh = (( temp >> 8 ) & 0xff );
			break;
		}
	}
	for (;;) {
		printf( "Sectors per track? [18-108]: " );
		gets( buf );
		if ( buf[0] == '\0' )
			continue;
		temp = numchk( buf, 18, 108 );
		if ( temp == (-1) ) {
			printf( "\n" );
			continue;
		} else {
			dbunit.db_spt = temp;
			break;
		}
	}
	for (;;) {
		printf( "Number of alternate cylinders? [0-50]: " );
		gets( buf );
		if ( buf[0] == '\0' )
			continue;
		temp = numchk( buf, 0, 50 );
		if ( temp == (-1) ) {
			printf( "\n" );
			continue;
		} else {
			if ( temp < (dbunit.db_cyls - temp) &&
			     (temp * dbunit.db_fheads) <= DLMAX &&
			     (temp * dbunit.db_rheads) <= DLMAX ) {
				dbunit.db_altcyls = temp;
				break;
			} else {
				printf( "\nToo many alternates!\n" );
				continue;
			}
		}
	}
	dl_f[i].dl_head = 0xff;
	for (;;) {
		printf( "Interleave factor? [1-20]: " );
		gets( buf );
		if ( buf[0] == '\0' )
			continue;
		temp = numchk( buf, 1, 20 );
		if ( temp == (-1) ) {
			printf( "\n" );
			continue;
		} else {
			dbdata.dbf_intl = temp;
			break;
		}
	}
	return(1);
}

/*
 * spare - main loop of the track sparing program.
 */

spare()
{
	char	tchar[32];
	int	i, j, k, temp;

	setdef();
	for ( i = 0; (i < DLMAX) && dbunit.db_fheads; ) {
		printf( "Tracks to spare on fixed? " );
		printf( "(cyl,head) [0-%d,0-%d]: ", (dbunit.db_cyls - dbunit.db_altcyls - 1),
			 (dbunit.db_fheads - 1) );
		gets( buf );
		if ( buf[0] == '\0' )
			break;
		for ( j = 0; j < 5; j++ ) {
			tchar[j] = buf[j];
			if ( tchar[j] == ',' ) {
				tchar[j] = '\0';
				break;
			}
			if ( tchar[j] == '\0' ) {
				j = 5;
				break;
			}
		}
		if ( j < 5 ) {
			temp = numchk( tchar, 0, (dbunit.db_cyls - dbunit.db_altcyls - 1) );
			if ( temp == (-1) ) 
				continue;
			else {
				dl_f[i].dl_cylinder = temp;
			}
		} else continue;
		j++;
		for ( k = 0; k < 2; k++, j++ ) {
			tchar[k] = buf[j];
			if ( tchar[k] == '\0' )
				break;
		}
		if ( k < 2 ) {
			temp = numchk( tchar, 0, (dbunit.db_fheads - 1) );
			if ( temp == (-1) )
				continue;
			dl_f[i].dl_head = temp;
			if ( i++ > dbunit.db_altcyls ) {
				i = 0;
				printf( "\nToo many defective tracks!\n" );
				continue;
			} else continue;
		} else continue;
	}
	dl_f[i].dl_head = 0xff;
	for ( i = 0; (i < DLMAX) && dbunit.db_rheads; ) {
		if ( spstate )
			break;
		else printf( "Defective tracks on removable? " );
		printf( "(cyl,head) [0-%d,0-%d]: ", dbunit.db_cyls,
			 dbunit.db_rheads );
		gets( buf );
		if ( buf[0] == '\0' )
			break;
		for ( j = 0; j < 5; j++ ) {
			tchar[j] = buf[j];
			if ( tchar[j] == ',' ) {
				tchar[j] = '\0';
				break;
			}
			if ( tchar[j] == '\0' ) {
				j = 5;
				break;
			}
		}
		if ( j < 5 ) {
			temp = numchk( tchar, 0, dbunit.db_cyls );
			if ( temp == (-1) ) 
				continue;
			else {
				dl_f[i].dl_cylinder = temp;
			}
		} else continue;
		j++;
		for ( k = 0; k < 2; k++, j++ ) {
			tchar[k] = buf[j];
			if ( tchar[k] == '\0' )
				break;
		}
		if ( k < 2 ) {
			temp = numchk( tchar, 0, dbunit.db_fheads );
			if ( temp == (-1) )
				continue;
			dl_f[i].dl_head = temp;
			if ( i++ > dbunit.db_altcyls ) {
				i = 0;
				printf( "\nToo many defective tracks!\n" );
				continue;
			} else continue;
		} else continue;
	}
	dl_r[i].dl_head = 0xff;
	if ( debug  ) {
		{	int	i,j;
			int	*iptr;
			iptr = (int *)dl_f;
			printf( "FIXED DEFECTIVES LIST\n" );
			for ( i=0; i<DLMAX/5; i++ ) {
				for ( j=0; j<10; j++ ) {
					printf( "%x, ", *iptr++ );
				}
			printf( "\n" );
			}
			mywaitgo();
			iptr = (int *)dl_r;
			printf( "REMOVABLE DEFECTIVES LIST\n" );
			for ( i=0; i<DLMAX/5; i++ ) {
				for ( j=0; j<10; j++ ) {
					printf( "%x, ", *iptr++ );
				}
			printf( "\n" );
			}
		}
	mywaitgo();
	}
	dinit();
	rdalt();
	chkalt();

	for ( i = 0; (i < DLMAX) && (dl_f[i].dl_head != 0xffff); i++ ) {
		savetrk( FIXED, dl_f[i].dl_cylinder, dl_f[i].dl_head );
		alt( FIXED, dl_f[i].dl_cylinder, dl_f[i].dl_head );
		resttrk( FIXED, dl_f[i].dl_cylinder, dl_f[i].dl_head );
	}
	for ( i = 0; (i < DLMAX) && (dl_r[i].dl_head != 0xffff); i++ ) {
		savetrk( REMOVABLE, dl_r[i].dl_cylinder, dl_r[i].dl_head );
		alt( REMOVABLE, dl_r[i].dl_cylinder, dl_r[i].dl_head );
		resttrk( REMOVABLE, dl_r[i].dl_cylinder, dl_r[i].dl_head );
	}
	wrtalt();
	printf( "All specified tracks are spared successfully!\n" );
}

/*
 * savetrk - saves the contents of a track being spared on the 'alternates
 *           track'.
 */

savetrk( type, cyl, head )
int	type, cyl, head;
{
	int	i;

	iop.i_unit = ( p_unit | ( type << 4 ) & p_both );
	iop.i_dboff = (unsigned)buf;
	iop.i_rc1 = (unsigned)0x200;
	for ( i = 0; i < dbunit.db_spt; i++ ) {
		iop.i_function = DF_READ;
		iop.i_cylinder = cyl;
		iop.i_head = head;
		iop.i_sector = i;
		cib.c_statsem = 0;
		if (fwaitdisk()) {
			printf( "Can't read sector %d of track %d,%d.\n",
				 i, iop.i_cylinder, iop.i_head );
			if ( debug ) {
				mywaitgo();
			}
		}
		if ( type == FIXED ) {
			iop.i_cylinder = afcyl;
			iop.i_head = afhead;
		} else {
			iop.i_cylinder = arcyl;
			iop.i_head = arhead;
		}
		iop.i_function = DF_WRITE;
		cib.c_statsem = 0;
		if (fwaitdisk()) {
			printf( "Can't write to save area.\n" );
			exit(1);
		}
	}
}

/*
 * resttrk - restores a track to the last track saved by savetrk.
 */

resttrk( type, cyl, head )
int	type, cyl, head;
{
	int	i;

	iop.i_unit = ( p_unit | ( type << 4 ) & p_both );
	iop.i_dboff = (unsigned)buf;
	iop.i_rc1 = (unsigned)0x200;
	for ( i = 0; i < dbunit.db_spt; i++ ) {
		iop.i_function = DF_READ;
		if ( type == FIXED ) {
			iop.i_cylinder = afcyl;
			iop.i_head = afhead;
		} else {
			iop.i_cylinder = arcyl;
			iop.i_head = arhead;
		}
		iop.i_sector = i;
		cib.c_statsem = 0;
		if (fwaitdisk()) {
			printf( "Can't read sector %d of track %d,%d.\n",
				 i, iop.i_cylinder, iop.i_head );
			if ( debug ) {
				mywaitgo();
			}
		}
		iop.i_cylinder = cyl;
		iop.i_head = head;
		iop.i_function = DF_WRITE;
		cib.c_statsem = 0;
		if (fwaitdisk()) {
			printf( "Can't write to alternate area.\n" );
			exit(1);
		}
	}
}

/*
 * chkalt - checks the specified list of defective tracks against the specified
 *          list of alternate tracks.  When a match is found that alternate 
 *          track is made unacailable for sparing.
 */

chkalt()
{
	int	i, j, k;

	for ( i = 0; (i < DLMAX) && (dl_f[i].dl_head != 0xffff); i++ ) {
		for ( j = 0; j < DLMAX; j++ ) {
			if ( ( dl_f[i].dl_cylinder == al_f[j].al_cyl ) &&
			     ( dl_f[i].dl_head == al_f[j].al_head ) ) {
				al_f[j].al_used = 0xff;
				for ( k = i; k < DLMAX; k++ ) {
					dl_f[k].dl_cylinder = dl_f[k+1].dl_cylinder;
					dl_f[k].dl_head = dl_f[k+1].dl_head;
				}
				i--;
				break;
			}
		}
	}
}

/*
 * format - main loop for disk formatting.
 */

format()
{

	int	i, j;
	setdef();
	getfsinfo();
	dinit();
	diagtrk();
	alttrk();
	chkalt();

	/*
	 * The controller has been initialized successfully, now we
	 * can format the desired unit.
	 */

	iop.i_function = DF_FORMAT;

	for ( i = 0; i < ( dbunit.db_cyls - dbunit.db_altcyls ); i++ ) {
		if ( dbunit.db_fheads != 0 ) {
			for ( j = 0; j < dbunit.db_fheads; j++ ) {
				if ( !defective( FIXED, i, j ) )
					fmt( FIXED, i, j );
				else alt( FIXED, i, j );
			}
		}
		if ( dbunit.db_rheads != 0 ) {
			for ( j = 0; j < dbunit.db_rheads; j++ ) {
				if ( !defective( REMOVABLE, i, j ) )
					fmt( REMOVABLE, i, j );
				else alt( REMOVABLE, i, j );
			}
		}
	}
	wrtalt();
	writezero();
	printf( "Disk formatted and initialized successfully!\n" );
}

/*
 * derror - disk error processing routine.
 */

derror()
{
	char buf[12];
	int i;


	debug = 1;
	debuginfo();
	iop.i_function = DF_STATUS;
	cib.c_statsem = 0;
	iop.i_dboff = (unsigned)buf;
	out_multibus( WUA, OP_START );
	for ( tocnt = MAXTOCNT; tocnt && (cib.c_statsem == 0); tocnt-- ) {
	}
	if ( !tocnt ) {
		printf( "Can't read status from controller\n" );
		exit(1);
	}
	printf("STATUS BLOCK = ");
	for (i=0; i<sizeof buf; i++)
		printf("%x, ",(buf[i^1]&0xff));
	printf("\n");
}
 
/*
 * fmt - formats the specified cylinder/head as a data track.
 */

fmt( type, cyl, head )
int	type, cyl, head;
{

	iop.i_function = DF_FORMAT;
	iop.i_unit = ( p_unit | ( ( type << 4 ) & p_both ) );
	iop.i_cylinder = cyl;
	iop.i_head = head;
	iop.i_dboff = (unsigned) &dbdata;
	cib.c_statsem = 0;
	debuginfo();
	if (fwaitdisk()) {
		printf( "Can't format %d, %d, %d\n", iop.i_unit,
						     iop.i_cylinder,
						     iop.i_head );
		derror();
		exit(1);
	}
	if ( debug ) {
		mywaitgo();
	}
	if ( debug )
		printf( "Formatted cyl = %x, head = %x\n", iop.i_cylinder,
						    (iop.i_head & 0xff) );
}

/*
 * alt - formats the specified cylinder/head as a defective track and 
 *       assigns and formats an alternate track for it.
 */

alt( type, cyl, head )
int	type, cyl, head;
{
	int	i;

	iop.i_function = DF_FORMAT;
	iop.i_unit = ( p_unit | (( type << 4 ) & p_both ));
	iop.i_cylinder = cyl;
	iop.i_head = head;
	iop.i_dboff = (unsigned)&dbdef;
	if ( type == FIXED ) {
		for ( i = 0; i < DLMAX; i++ ) {
			if ( !(al_f[i].al_used) )
				break;
		}
		if ( i == DLMAX ) {
			printf( "Out of fixed alternate tracks\n" );
			exit(1);
		}
		al_f[i].al_used = 0xff;
		dbdef.dbf_up2 = al_f[i].al_head;
		dbdef.dbf_up1 = ( al_f[i].al_cyl & 0xff );
		dbdef.dbf_up3 = (( al_f[i].al_cyl >> 8 ) & 0xff );
		dbdef.dbf_up4 = 0;
	} else {
		for ( i = 0; i < DLMAX; i++ ) {
			if ( !(al_r[i].al_used) )
				break;
		}
		if ( i == DLMAX ) {
			printf( "Out of removable alternate tracks\n" );
			exit(1);
		}
		al_r[i].al_used = 0xff;
		dbdef.dbf_up2 = al_r[i].al_head;
		dbdef.dbf_up1 = ( al_r[i].al_cyl & 0xff );
		dbdef.dbf_up3 = (( al_r[i].al_cyl >> 8 ) & 0xff );
		dbdef.dbf_up4 = 0;
	}
	cib.c_statsem = 0;
	debuginfo();
	if (fwaitdisk()) {
		printf( "Can't format %d, %d, %d\n", iop.i_unit,
						     iop.i_cylinder,
						     iop.i_head );
		exit(1);
	}
	if ( debug )
		printf( "Defective track ( cyl = %x, head = %x ) formatted\n",
			iop.i_cylinder, iop.i_head );
	iop.i_cylinder = (( dbdef.dbf_up3 << 8 ) | dbdef.dbf_up1 );
	iop.i_head = dbdef.dbf_up2;
	iop.i_dboff = (unsigned)&dbalt;
	cib.c_statsem = 0;
	debuginfo();
	if (fwaitdisk()) {
		printf( "Can't format %d, %d, %d\n", iop.i_unit,
						     iop.i_cylinder,
						     iop.i_head );
		exit(1);
	}
	if ( debug )
		printf( "Alternate track (cyl = %x, head = %x) formatted.\n",
			iop.i_cylinder, iop.i_head );
}

/*
 * defective - formats the specified cylinder/head as a defective track.
 */

defective( type, cyl, head )
int	type, cyl, head;
{
	int	i;

	if ( debug ) {
		printf( "Checking cyl = %x, head = %x\n", cyl, head );
		mywaitgo();
	}
	if ( type == FIXED ) {
		for ( i = 0; (i < DLMAX) && (dl_f[i].dl_head != 0xffff); i++ ) {
			if ( debug ) {
				printf( "Current entry cyl = %x, head = %x\n",
					 dl_f[i].dl_cylinder, dl_f[i].dl_head );
				mywaitgo();
			}
			if ( (cyl == dl_f[i].dl_cylinder) && (head == dl_f[i].dl_head) )
				return( 1 );
		}
	} else {
		for ( i = 0; (i < DLMAX) && (dl_r[i].dl_head != 0xff); i++ ) {
			if ( debug ) {
				printf( "Current entry cyl = %x, head = %x\n",
					 dl_r[i].dl_cylinder, dl_r[i].dl_head );
				mywaitgo();
			}
			if ( (cyl == dl_r[i].dl_cylinder) && (head == dl_r[i].dl_head) )
				return( 1 );
		}
	}
	return( 0 );
}

/*
 * main - main loop for dformat.  Calls user selected portion of dformat.
 */

main()
{
	register i;

	printf( "$$ dformat \n" );

	out_local( 0x80bf, 0x10);
	out_local( 0x80ff, 0x10);

	debug = 0;
	do {
		printf( "Format, spare, or initialize the disk? [fsi]: " );
		gets( buf );
	} while (( ( buf[0] != 'f' ) && ( buf[0] != 's' ) && (buf[0] != 'i')
		&& (buf[0] != 'l') && (buf[0] != 'r')) || (buf[1] != '\0') );
	do {
		printf( "Unit? [pd(0-3,0) or is(0-3,0)]: " );
		gets( &buf[10] );
		for (i=10; i<50; i++)
			if (buf[i] == '(')
				break;
		if (buf[10] == '?')
			{
			debug = 1;
			continue;
			}
		if ((buf[10] == 'p') || (buf[10] == 'd')) {
			imain(buf);
			exit(0);
		}
	} while ((( buf[i+1] < '0' ) || ( buf[i+1] > '3' )) || ( buf[10] != 'i' ));
	p_unit = atol( &buf[i+1] );
	if ( buf[0] == 's' ) {
		spstate++;
		spare();
	} else if ( buf[0] == 'i') {
		spstate = 0;
		reinit();
	} else if ( buf[0] == 'f') {
		spstate = 0;
		format();
	} else {
		printf("Illegal option for %s\n",&buf[10]);
		exit(1);
	}
	exit(0);
}

debuginfo() {
	int i,j;
	int *iptr;

	if (debug) {
		iptr = (int *)&ccb;
		printf( "CCB = " );
		for (i=0; i<8; i++) {
			printf("%x, ",*iptr++);
		}
		printf("\n");
		iptr = (int *)&cib;
		printf( "CIB = " );
		for (i=0; i<8; i++) {
			printf("%x, ",*iptr++);
		}
		printf("\n");
		iptr = (int *)&iop;
		printf( "IOP = " );
		for (i=0; i<15; i++) {
			printf("%x, ",*iptr++);
		}
		printf( "\n" );
		iptr = (int *)iop.i_dboff;
		printf( "DATA = " );
		for (i=0; i<4; i++) {
			printf("%x, ",*iptr++);
		}
		printf( "\n" );

		mywaitgo();
	}
}

fwaitdisk() {
	out_multibus(WUA, OP_START);
	for (tocnt = MAXTOCNT; tocnt && (cib.c_statsem == 0); tocnt--) ;
	return( !tocnt || (cib.c_opstat & ERRFLAGS));
}

