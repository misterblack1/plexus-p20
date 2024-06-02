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
#ifdef pdp11
	int	v_cmap;
#endif
	int	v_smap;
	int	v_hbuf;
	int	v_hmask;
};
extern struct var v;
