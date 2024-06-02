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
#define TDEV(x)	(x&017)
#define BDEV(x)	((x>>4)&03)
#define KDEV(x)	(x&0300)
#define NORESP		15
#define RRTNEBUF	16
#ifdef vax
#define MOVE	iomove
#define GETBLK	geteblk()
#else
#define MOVE	pimove
#define GETBLK	getablk(0)
#endif
