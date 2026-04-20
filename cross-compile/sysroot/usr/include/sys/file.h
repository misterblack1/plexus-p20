/* SID @(#)file.h	5.1 */
/* @(#)file.h	6.1 */
/*
 * One file structure is allocated for each open/creat/pipe call.
 * Main use is to hold the read/write pointer associated with
 * each open file.
 */
struct	file
{
	char	f_flag;
	cnt_t	f_count;	/* reference count */
	union {
		struct inode *f_uinode;	/* pointer to inode structure */
		struct file  *f_unext;	/* next entry in freelist */
	} f_up;
	union {
		off_t	f_off;		/* read/write character pointer */
	} f_un;
};

#define	f_inode		f_up.f_uinode
#define	f_next		f_up.f_unext
#define	f_offset	f_un.f_off

extern struct file file[];	/* The file table itself */
extern struct file *ffreelist;	/* Head of freelist pool */

/* flags */
#define	FOPEN	(-1)
#define	FREAD	00001
#define	FWRITE	00002
#define	FNDELAY	00004
#define	FAPPEND	00010
#define	FSYNC	00020
#define FCONC	00040		/* concurrent io JF */
#define FBDMA	00100		/* use block dma JF */
#define	FMASK	00377

/* open only modes */
#define	FCREAT	00400
#define	FTRUNC	01000
#define	FEXCL	02000
