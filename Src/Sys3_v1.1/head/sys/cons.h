/*
 *	VAX console interface registers
 */

#ifdef vax
#define	C_IE	0x0040	/* interrupt enable */
#define	C_DONE	0x0080	/* done */
#define	C_CID	0x0f00	/* channel id */
#define	C_DATA	0x00ff	/* data byte */
#endif

/*
 *	Floppy interface
 */

#ifdef vax
#define	F_DATA	0x0100	/* data byte */
#define	F_FFC	0x0200	/* function complete */
#define	F_FLP	0x0900	/* floppy command */
#define	F_READ		0x0900	/* read sector */
#define	F_WRITE		0x0901	/* write sector */
#define	F_STAT		0x0902	/* get status */
#define	F_DEL		0x0903	/* write deleted sector */
#define	F_CAN		0x0904	/* cancel command */
#define	F_PERR		0x0905	/* protocol error */

#define	FTRK	77	/* tracks */
#define	FSEC	26	/* sectors */
#define	FSIZE	128	/* bytes */
#endif
