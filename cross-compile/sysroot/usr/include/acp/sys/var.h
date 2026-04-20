/* SID */
/* @(#)var.h	5.1 4/22/86 */

struct var {
	int	v_buf;
	int	v_call;
	int	v_file;
	char *	ve_file;
	int	v_proc;
	char *	ve_proc;
	int	v_text;
	char *	ve_text;
	int	v_clist;
	int	v_cmap;
	int	v_dmap;
	int	v_iosize;
};
extern struct var v;
