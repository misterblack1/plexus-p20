
/* SID @(#)vpmd.h	5.1 */
/*	vpmd.h	1.0 11/10/84 */

/*Buffer discriptor structure*/

struct vpmbd {
	short	d_ct;		/*Character count or buffer size*/
	short	d_adres;	/*Address; low order 16 bits*/
	char	d_hbits;	/*Address; high order 2 bits*/
	char	d_type;		/*Used for info transfer vpm<->kmc*/
	char	d_sta;		/*Station address*/
	char	d_dev;		/*Device address*/
	struct buf *d_buf;	/*Pointer to system buffer descriptor*/
	int	d_bos;		/*Index of next byte in buffer*/
	int	d_vpmtdev;
};
#ifndef ICP

#define VPMBS   4               /* Number of Virtual Protocol Machines */
#define NUMVICP	5		/* Max number of ICPs */

/* Per vpm structure */
struct vpmb {
	short    vb_state;     /* status info */
	short    vb_howmany;   /* How many lines opened this protocol */
	int      (*vb_rint)(); /* Called upon receive interrupt */
	int	 vb_device;
	char     vb_rpt[4];
	char     vb_errs[8];
} vpmb[VPMBS * NUMVICP];
#endif
