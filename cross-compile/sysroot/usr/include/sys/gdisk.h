/* SID @(#)gdisk.h	5.1 */
/* @(#)gdisk.h	6.1 */
/*
 * VAX general disk header
 */
struct device {
	int	cs1, ds, er1, mr;
	int	as, da, dt, la;
	int	sn, of, dc, cc;
	int	er2, er3, ec1, ec2;
};

extern struct mba *gd_addr[];
extern	gd_cnt;

#define	gdctl(X)	((X>>6)&3)
#define	gddrive(X)	((X>>3)&7)
#define	gdslice(X)	(X&7)
#define	gdstod(X)	(X<<3)
#define	gdpos(X)	((X>>3)&037)
#define	gdmkpos(X,Y)	((X<<3)|Y)
#define	gdptoc(X)	(X>>3)
#define	gdptod(X)	(X&7)
#define	gdloc(X)	gdsw[gdindex[X]]

struct d_sizes {
	daddr_t nblocks;
	int	cyloff;
};

struct gdsw {
	short	type;	/* hardware drive type */
	char *	name;	/* printf name */
	short	trk;
	short	sec;
	struct d_sizes *sizep;
	int	(*open)();
	int	(*strategy)();
	int	(*intr)();
	int	(*start)();
};

extern struct gdsw gdsw[];
extern int gdindex[];	/* array of indices into gdsw */
extern short gdtype[];	/* array of drive types */
extern struct iotime gdstat[];
extern struct iobuf gdtab[], gdutab[];

#define	GO	01
#define	RECAL	06
#define	DCLR	010
#define	RELEASE	012
#define	PRESET	020
#define	SEARCH	030
#define	WCOM	060
#define	RCOM	070

#define	ERR	040000	/* ds - Error */
#define	MOL	010000	/* ds - Medium online */
#define	VV	0100	/* ds - volume valid */
#define	INTLV	04	/* ds - RP07 interleave */
#define	RDY	0200	/* cs1 - Ready */
#define	WLE	04000	/* er1 - write lock error */
#define	DCK	0100000	/* er1 - Data check */
#define SSE	040	/* er3 - Skip Sector Error */
#define	NED	0x40000	/* mbasr - Non-Existent Device */
#define	FMT22	010000	/* of - 16 bit/word format */
#define	ECI	04000	/* of - ecc inhibit */
#define SSEI	01000	/* of - SSE inhibit */
#define DRQ	04000	/* dt - dual port option */
#define	DTYPE	0777	/* dt - drive type */

#define	wtime	b_flags
#define	WOK	0
#define	WABORT	1
#define	WNED	2
#define	WMOL	4
#define	WDVA	6
#define	WERR	30

#define	acts	io_s1
#define	qcnt	io_s2

#define	trksec	av_back
#define	cylin	b_resid

#define	GDPRI	(PZERO + 3)

/* supported types */
#define	GDRP04	020
#define	GDRP05	021
#define	GDRP06	022
#define	GDRM80	026
#define	GDRP07	042
#define	GDRM05	027
