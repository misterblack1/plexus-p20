struct vp {
	int	vp_state;
	struct buf *vp_buf;
	ushort vp_count;
	int	vp_offset;
};
extern struct vp vp_vp[];

struct da {
	struct buf *da_bp;
	int	da_cnt, da_off;
	int	da_cs, da_db;
	struct proc	*da_proc;
	int	da_flags;
};
extern struct da da_da[];

struct du {
	char	*du_buf;
	char	*du_bufp;
	int	du_nxmit;
	char	du_state;
	char	du_dummy;
	struct proc	*du_proc;
	char	*du_bufb;
	char	*du_bufe;
	int	du_nleft;
};
extern struct du du_du[];
