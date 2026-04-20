/* SID @(#)vp.h	5.1 */
/* @(#)vp.h	6.1 */
struct vp {
	int	vp_state;
	struct buf	*vp_buf;
	ushort	vp_count;
	int	vp_offset;
};
extern struct vp vp_vp[];
