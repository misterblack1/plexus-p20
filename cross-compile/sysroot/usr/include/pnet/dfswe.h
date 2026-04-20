/* SID */
/* @(#)dfswe.h	5.1 4/22/86 */

/*
 *  Dfs work entry format.
 */

struct	dfswe	{
	struct dfswe *we_link;		/* link to next entry in chain */
	char	we_type;		/* type of work to do */
	char	we_stat;		/* status of work entry */
	char	we_error;		/* additional error status */
	union {
		struct wemnt {
			struct	dfsmnt	*we_mmsg; /* dfsmnt msg pointer */
			ushort	we_mip; 	/* mounted inode ordinal */
			struct	dqp	*we_mdpro; /* process pointer */
		} wm;
		struct wegen {
			ushort	weg_ip;		/* inode ordinal */
			ushort	weg_flag;	/* general flag */
		} weg;
		struct wenami {
			struct	dfsdfs	*we_nmsg; /* namei msg pointer */
			struct	dfsdfs	*we_nrmsg; /* ret. msg pointer */
			ushort	we_nip; /* local inode ordinal */
		} wn;
		struct	wesis {
			char	we_sname[9];
			char	we_inher;
			ushort	we_ufp[NOFILE];
			daddr_t	we_ulimit;
			char    we_user[6];
			char    we_upass[13];
			char	we_grp[6];
			char    we_gpass[13];
		} ws;
		struct	wechmod {
			ushort	we_cip; /*remote inode pointer */
			ushort	we_cmode;	 /*mode for change */
			char    we_cuser[6];      /* user name */
			char    we_cupass[13];    /* user password */
			char    we_cgrp[6];       /* group name */
			char    we_cgpass[13];    /* group password */
		} wc;
		struct	weutime {
			ushort	we_uip; 	/*remote inode ordinal */
			time_t	we_utacc;	/* access time */
			time_t	we_utmod;	/* modified time */
		} wu;
		struct	wechown {
			ushort	we_coip; 	/*remote inode ordinal */
			ushort  we_couid;       /* return user id */
			ushort  we_cogid;       /* return group id */
			ushort  we_couuid;      /* process uuid */
			ushort  we_coimode;     /* mode for change */
			char    we_couser[6];	/* user name */
			char    we_coupass[13]; /* user password */
			char	we_cogrp[6];	/* group name */
			char    we_cogpass[13]; /* group password */
			char    we_conuser[6];	/* new user name */
			char	we_conpassu[13]; /* new user password */
			char	we_congrp[6];	/* new group name */
			char	we_conxpassg[13]; /* new group password */
		} wco;
		struct westat1 {
			ushort	we_s1ip; 	/* remote inode ordinal */
			struct	stat	*we_sub; /* user stat structure */
			struct dfsdfs	*we_rmsg; /* returned message */
		} ws1;
		struct wemknod {
			ushort	we_mkip; /* remote inode pointer */
			struct	dfsdfs	*we_mkmsg; /* msg pointer */
			struct	dfswe	*we_mkdwent; /* work entry */
			struct	dfsdfs	*we_mkrmsg; /* ret. msg pointer */
		} wmkn;
		struct weunlink {
			ushort	we_ulip; /* remote inode pointer */
			struct	dfsdfs	*we_ulmsg; /* msg pointer */
			struct	dfswe	*we_uldwent; /* work entry */
		} wunl;
		struct werdwr {
			struct	dfsrdwr	*we_rwmsg; /* msg pointer */
			struct	dfswe	*we_rwdwent; /* work entry */
		} wrdwr;
		struct weacc {
			struct	dfsdfs *we_accmsg;  /*msg pointer */
			ushort	we_accip;	   /* remode inode pointer */
			ushort 	we_accmode;  	   /* mode */
		} wacc;
		struct weitr {
			ushort  witr_ip;	   /* remote inode pointer */
			struct  dfsdfs *witr_msg;  /* msg pointer */
		} witr;
		struct weissr {
			char  *wissr_buf;	   /* message buffer address */
			ushort  wissr_rtype;  	/* response type */
		} wissr;
	} weu;
};

#define wm_type	we_type
#define wm_stat	we_stat
#define wm_error	we_error
#define wm_msg	weu.wm.we_mmsg
#define wm_ip	weu.wm.we_mip
#define wm_dpro weu.wm.we_mdpro

#define wo_type	we_type
#define wo_stat	we_stat
#define wo_error	we_error
#define wo_ip	weu.weg.weg_ip
#define wo_flag	weu.weg.weg_flag

#define wn_type	we_type
#define wn_stat	we_stat
#define wn_error	we_error
#define wn_msg	weu.wn.we_nmsg
#define wn_rmsg	weu.wn.we_nrmsg
#define wn_ip	weu.wn.we_nip

#define wi_type	we_type
#define wi_stat	we_stat
#define wi_error	we_error
#define wi_ip	weu.weg.weg_ip

#define wic_type	we_type
#define wic_stat	we_stat
#define wic_error	we_error
#define wic_ip	weu.weg.weg_ip
#define wic_flg	weu.weg.weg_flag

#define wp_type	we_type
#define wp_stat	we_stat
#define wp_error	we_error
#define wp_ip	weu.weg.weg_ip

#define wpl_type	we_type
#define wpl_stat	we_stat
#define wpl_error	we_error
#define wpl_ip	weu.weg.weg_ip

#define ws_type	we_type
#define ws_stat	we_stat
#define ws_error	we_error
#define ws_rname	weu.ws.we_sname
#define ws_inher	weu.ws.we_inher
#define ws_ufp		weu.ws.we_ufp
#define ws_ulimit	weu.ws.we_ulimit
#define ws_user         weu.ws.we_user
#define ws_upass        weu.ws.we_upass
#define ws_grp          weu.ws.we_grp
#define ws_gpass        weu.ws.we_gpass

#define wc_type	we_type
#define wc_stat	we_stat
#define wc_error	we_error
#define wc_ip	weu.wc.we_cip
#define wc_mode	weu.wc.we_cmode
#define wc_user	weu.wc.we_cuser
#define wc_upass weu.wc.we_cupass
#define wc_grp   weu.wc.we_cgrp
#define wc_gpass weu.wc.we_cgpass

#define wu_type	we_type
#define wu_stat	we_stat
#define wu_error	we_error
#define wu_ip	weu.wu.we_uip
#define wu_tacc	weu.wu.we_utacc
#define wu_tmod	weu.wu.we_utmod

#define wco_type	we_type
#define wco_stat	we_stat
#define wco_error	we_error
#define wco_ip	weu.wco.we_coip
#define wco_uid	weu.wco.we_couid
#define wco_gid	weu.wco.we_cogid
#define wco_uuid	weu.wco.we_couuid
#define wco_mode	weu.wco.we_coimode
#define wco_user	weu.wco.we_couser
#define wco_upass 	weu.wco.we_coupass
#define wco_grp		weu.wco.we_cogrp
#define wco_gpass 	weu.wco.we_cogpass
#define wco_nuser 	weu.wco.we_conuser
#define wco_npassu 	weu.wco.we_conpassu
#define wco_ngrp 	weu.wco.we_congrp
#define wco_nxpassg 	weu.wco.we_conxpassg

#define ws1_type	we_type
#define ws1_stat	we_stat
#define ws1_error	we_error
#define ws1_ip	weu.ws1.we_s1ip
#define ws1_ub	weu.ws1.we_sub
#define ws1_rmsg	weu.ws1.we_rmsg

#define wmn_type	we_type
#define wmn_stat	we_stat
#define wmn_error	we_error
#define wmn_ip	weu.wmkn.we_mkip
#define wmn_dwe	weu.wmkn.we_mkdwent
#define wmn_msg	weu.wmkn.we_mkmsg
#define wmn_rmsg weu.wmkn.we_mkrmsg

#define wul_type	we_type
#define wul_stat	we_stat
#define wul_error	we_error
#define wul_ip	weu.wunl.we_ulip
#define wul_dwe	weu.wunl.we_uldwent
#define wul_msg	weu.wunl.we_ulmsg

#define wrw_type	we_type
#define wrw_stat	we_stat
#define wrw_error	we_error
#define wrw_dwe	weu.wrdwr.we_rwdwent
#define wrw_msg	weu.wrdwr.we_rwmsg

#define wit_type	we_type
#define wit_stat	we_stat
#define wit_error	we_error
#define wit_ip		weu.witr.witr_ip
#define wit_msg		weu.witr.witr_msg

#define wr_type	we_type
#define wr_stat	we_stat
#define wr_error	we_error
#define wr_buff		weu.wissr.wissr_buf
#define wr_rtype	weu.wissr.wissr_rtype

#define wacc_type	we_type
#define wacc_stat	we_stat
#define wacc_error	we_error
#define wacc_msg        weu.wacc.we_accmsg
#define wacc_ip		weu.wacc.we_accip 
#define wacc_mode	weu.wacc.we_accmode

