/* SID @(#)var.h	5.1 */
/* @(#)var.h	6.1 */
struct var {
	int	v_buf;
	int	v_call;
	int	v_inode;
	char *	ve_inode;
	int	v_file;
	char *	ve_file;
	int	v_mount;
	char *	ve_mount;
	int	v_proc;
	char *	ve_proc;
	int	v_text;
	char *	ve_text;
	int	v_clist;
	int	v_sabuf;
	int	v_maxup;
	int	v_smap;
	int	v_hbuf;
	int	v_hmask;
	int	v_pbuf;
	int	v_nhosts;
	char *	ve_nhosts;
	int	v_nattach;
	char *	ve_nattach;
	int	v_nvc;
	char *	ve_nvc;
	int	v_nrmount;
	char *	ve_rmntab;
	int	v_ndfswe;
	int	v_ndfspro;
	long	v_cdlimit;
	int	v_nmaxblk;
	int	v_cmaxblk;
	int	v_cacheblk;
	int	v_ftbufsize;
};
extern struct var v;
