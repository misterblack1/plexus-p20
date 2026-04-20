/*  */
/* SID @(#)cb.h	5.1 */
/* @(#)cb.h	1.6 6/14/83 */



	/*-------------------------------------------------------------*\
	| Communication block. Read data format as seen by PDLC and NCF |
	\*-------------------------------------------------------------*/

struct ds {
#ifdef m68
	unsigned int ds_adr;	/* address descripter */
#else
	ushort ds_seg;		/* segment descripter */
	ushort ds_adr;		/* address descripter */
#endif
	ushort ds_len;		/* length descripter */
};

struct cb {
	struct cb *cb_next;	/* pointer to next communication block */
	struct ds  cb_des[8];	/* descripter field from dfs layer */
	ushort	   cb_src;	/* id of source */
	ushort	   cb_dest;	/* id of destination. */

	ushort	   cb_lnk;	/* type of link msg (pdlc, ip, lapb ...) */
	ushort 	   cb_type;	/* type of pdlc msg (address|type) */
	ushort	   cb_rsv1[2];	/* resevered for future use */
	ushort	   cb_net;	/* type of network msg (ncf/tcp.ip ...) */
	ushort	   cb_rsv2[2];	/* resevered for future use */
	ushort	   cb_size;	/* total size of cb + ncf/dfs data + usr data*/
	ushort 	   cb_seq;	/* sequence field for multi block msgs */
	ushort	   cb_ntype;	/* type of ncf msg */
	union {
		struct {
			unsigned char cb_a;
			unsigned char cb_c;
		} cb_b;
		ushort cb_d;
	} cb_e;
	ushort	   cb_chan;	/* connection sequence number */
	ushort	   cb_len;	/* length of user data when used */
};

#define cb_slot cb_e.cb_d
#define cb_atch cb_e.cb_b.cb_a
#define cb_conn cb_e.cb_b.cb_c
