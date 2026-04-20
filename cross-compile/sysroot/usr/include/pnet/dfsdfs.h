/* SID */
/* @(#)dfsdfs.h	5.1 4/22/86 */

/*
 *  Mounted directory request message format.
 */

struct	dfsdfs {
	struct	dfshdr	dfs_hdr;	/* dfs message header */
	union {				/* various function msg formats */
		struct	{
			char	nm_path[DFSMAXP];	
					/* path name for remote name */
			char	nm_flag;		
					/* name find, create, delete flag */
			ushort  nm_uid;         /* return user id */
			ushort  nm_gid;         /* return grp id */
			ushort	nm_ncdir; /* users current direct */
			char	nm_iflag;	/* inode flags */
			char	nm_fflag;	/* file system flags */
			ushort	nm_mode;	/* inode mode */
			struct	dqp	*nm_mdpro; /* mount process pointer */
			ushort	nm_inode;
					/* local inode ordinal */
			ushort	nm_rinode;
					/* return remote inode ordinal */
			ushort	nm_rpdir;
					/* parent directory inode */
			off_t	nm_roffset; /* parent direct. offset */
			char	nm_rdname[DIRSIZ]; /* directory name */
			ino_t	nm_rdino;	/* directory i number */
			ushort	nm_pcnt;	/* partial namei count */
			char	nm_done;	/* remote namei done flag */
			char    nm_user[6];     /* user name */
			char    nm_upass[13];   /* user password */
			char    nm_grp[6];      /* group name */
			char	nm_gpass[13];   /* group password */
		} nm;
		struct {
			ushort 	g_file;		/* remote inode or filep */
			ushort	g_flag1;	/* flag 1 word */
			ushort	g_flag2;	/* flag 2 word */
			ushort	g_flag3;	/* flag 3 word */
		} gen;
		struct {
			ushort	itr_inode;	/* remote inode */
			char    itr_user[6];	/* user name */
			char	itr_upass[13];  /* user password */
			char	itr_grp[6];	/* group name */
			char	itr_gpass[13];  /* group password */
		} it;
		struct {
			char	s_node[9];	/* node for sister proc. */
			char	s_name[9];	/* process for sister */
			char	s_inher;	/* inherit flag */
			ushort	s_ufp[NOFILE]; /* file pointers */
			daddr_t	s_ulimit;	/* file ulimit */
			char    s_user[6];      /* user name */
			char    s_upass[13];    /* user password */
			char    s_grp[6];       /* grp name */
			char	s_gpass[13];    /* grp password */
		} ss;
		struct {
			ushort	ut_ino; 	/* remote inode ordinal */
			time_t	ut_ta;		/* access time */
			time_t	ut_tmd;		/* modified time */
		} ut;
		struct {
			ushort	co_ino; 	/* remote inode ordinal */
			ushort	co_coimode;	/* inode mode to change */
			ushort  co_couid;       /* return user id */
			ushort  co_cogid;       /* return grp id */
			ushort	co_couuid;	/* process user id */
			char	co_couser[6];	/* user name */
			char    co_coupass[13]; /* user password */
			char	co_cogrp[6];	/* group name */
			char    co_cogpass[13]; /* group password */
			char	co_conuser[6];	/* new user name */
			char	co_conpassu[13]; /* new user password */
			char	co_congrp[6];	/* new group name */
			char	co_conxpassg[13]; /* new group password */
		} co;
		struct {
			ushort	st1_st1ip; /* remote inode pointer */
			struct	stat	st1_stat; /* return stat buffer */
		} s1;
		struct {
			ushort	mkn_inode;	/* inode ptr */
			ushort	mkn_rinode;	/* rmt inode ptr */
			ushort	mkn_mrdev;	/* i_rdev if chr or blk */
			ushort  mkn_muid;	/* return user id */
			ushort  mkn_mgid;	/* return group id */
			char	mkn_miflag;	/* inode flags */
			char	mkn_mfflag;	/* file system flags */
			ushort	mkn_mimode;	/* inode mode */
			ushort	mkn_mcmask;	/* u.u_cmask */
			ushort	mkn_mmode;	/* makenode mode */
			off_t	mkn_moffset; /* parent direct. offset */
			char	mkn_mdname[DIRSIZ]; /* directory name */
			ino_t	mkn_mdino;	/* directory i number */
			char    mkn_muser[6];   /* user name */
			char    mkn_mupass[13]; /* user password */
			char    mkn_mgrp[6];    /* group name */
			char    mkn_mgpass[13]; /* group password */
		} mknd;
		struct {
			ushort	unl_inode;	/* inode ordinal */
			off_t	unl_uoffset; /* parent direct. offset */
			ushort	unl_updir;	/* parent directory ord.*/
			ino_t	unl_udino;	/* directory i number */
			char	unl_udname[DIRSIZ]; /* directory name */
			char    unl_uuser[6];   /* user name */
			char    unl_uupass[13]; /* user password */
		} unlk;
		struct {
			ushort	sk_filp; 	/* remote file pointer ord. */
			off_t	sk_soff;	/* seek offset */
			ushort	sk_sbase;	/* seek whence */
			ushort	sk_ssig;	/* signal indicator */
		} sk;
		struct {
			ushort  acc_afilp;	/* remote file pointer ord. */
			ushort  acc_amode;	/* requested mode */
			char    acc_auser[6];	/* user name */
			char    acc_aupass[13]; /* user password */
			char    acc_agrp[6];    /* group name */
			char    acc_agpass[13]; /* group password */
		} acc;
	} fm;
};

#define	op_type	dfs_hdr.dh_type
#define op_stat	dfs_hdr.dh_stat
#define op_error dfs_hdr.dh_error
#define op_len	dfs_hdr.dh_len
#define op_dwe	dfs_hdr.dh_dwent
#define op_inode fm.gen.g_file
#define op_fp	fm.gen.g_flag1
#define op_flag	fm.gen.g_flag2
#define op_pflag fm.gen.g_flag3

#define	cl_type	dfs_hdr.dh_type
#define cl_stat	dfs_hdr.dh_stat
#define cl_error dfs_hdr.dh_error
#define cl_len	dfs_hdr.dh_len
#define cl_dwe	dfs_hdr.dh_dwent
#define cl_inode fm.gen.g_file
#define cl_fp	fm.gen.g_flag1
#define cl_flag	fm.gen.g_flag2

#define	nmi_type	dfs_hdr.dh_type
#define nmi_stat	dfs_hdr.dh_stat
#define nmi_error dfs_hdr.dh_error
#define nmi_len	dfs_hdr.dh_len
#define nmi_dwe	dfs_hdr.dh_dwent
#define nmi_path	fm.nm.nm_path
#define nmi_flag	fm.nm.nm_flag
#define nmi_uid	fm.nm.nm_uid
#define nmi_gid	fm.nm.nm_gid
#define nmi_cdir	fm.nm.nm_ncdir
#define nmi_iflag	fm.nm.nm_iflag
#define nmi_fflag	fm.nm.nm_fflag
#define nmi_mode	fm.nm.nm_mode
#define nmi_dpro	fm.nm.nm_mdpro
#define nmi_inode	fm.nm.nm_inode
#define nmi_rinode	fm.nm.nm_rinode
#define nmi_pdir	fm.nm.nm_rpdir
#define nmi_offset	fm.nm.nm_roffset
#define nmi_dname	fm.nm.nm_rdname
#define nmi_dino	fm.nm.nm_rdino
#define nmi_pcnt	fm.nm.nm_pcnt
#define nmi_done	fm.nm.nm_done
#define nmi_user        fm.nm.nm_user
#define nmi_upass	fm.nm.nm_upass
#define nmi_grp		fm.nm.nm_grp
#define nmi_gpass	fm.nm.nm_gpass

#define	ip_type	dfs_hdr.dh_type
#define ip_stat	dfs_hdr.dh_stat
#define ip_error dfs_hdr.dh_error
#define ip_len	dfs_hdr.dh_len
#define ip_dwe		dfs_hdr.dh_dwent
#define ip_inode	fm.gen.g_file

#define	cip_type	dfs_hdr.dh_type
#define cip_stat	dfs_hdr.dh_stat
#define cip_error dfs_hdr.dh_error
#define cip_len	dfs_hdr.dh_len
#define cip_dwe		dfs_hdr.dh_dwent
#define cip_inode	fm.gen.g_file
#define cip_flg	fm.gen.g_flag1

#define	it_type	dfs_hdr.dh_type
#define it_stat	dfs_hdr.dh_stat
#define it_error dfs_hdr.dh_error
#define it_len	dfs_hdr.dh_len
#define it_dwe		dfs_hdr.dh_dwent
#define it_inode	fm.it.itr_inode
#define it_user		fm.it.itr_user
#define it_upass	fm.it.itr_upass
#define it_grp		fm.it.itr_grp
#define it_gpass	fm.it.itr_gpass

#define	sis_type	dfs_hdr.dh_type
#define sis_stat	dfs_hdr.dh_stat
#define sis_error dfs_hdr.dh_error
#define sis_dwe	dfs_hdr.dh_dwent
#define sis_name	fm.ss.s_name
#define sis_node	fm.ss.s_node
#define sis_inher	fm.ss.s_inher
#define sis_ufp		fm.ss.s_ufp
#define sis_ulimit	fm.ss.s_ulimit
#define sis_user	fm.ss.s_user
#define sis_upass	fm.ss.s_upass
#define sis_grp		fm.ss.s_grp
#define sis_gpass	fm.ss.s_gpass

#define	fc_type	dfs_hdr.dh_type
#define fc_stat	dfs_hdr.dh_stat
#define fc_error dfs_hdr.dh_error
#define fc_fp	fm.gen.g_file
#define fc_cmd	fm.gen.g_flag1
#define fc_arg	fm.gen.g_flag2
#define fc_fdn	fm.gen.g_flag3

#define acc_type	dfs_hdr.dh_type
#define acc_stat	dfs_hdr.dh_stat
#define acc_error	dfs_hdr.dh_error
#define acc_dwe		dfs_hdr.dh_dwent
#define acc_inode   	fm.acc.acc_afilp
#define acc_mode   	fm.acc.acc_amode
#define acc_user   	fm.acc.acc_auser
#define acc_upass   	fm.acc.acc_aupass
#define acc_grp   	fm.acc.acc_agrp
#define acc_gpass   	fm.acc.acc_agpass

#define	sk_type	dfs_hdr.dh_type
#define sk_stat	dfs_hdr.dh_stat
#define sk_error dfs_hdr.dh_error
#define sk_fp	fm.sk.sk_filp
#define sk_off	fm.sk.sk_soff
#define sk_base	fm.sk.sk_sbase
#define sk_sig	fm.sk.sk_ssig

#define	cm_type	dfs_hdr.dh_type
#define cm_stat	dfs_hdr.dh_stat
#define cm_error dfs_hdr.dh_error
#define cm_len	dfs_hdr.dh_len
#define cm_dwe	dfs_hdr.dh_dwent
#define cm_ip fm.co.co_ino
#define cm_mode fm.co.co_coimode
#define cm_user fm.co.co_couser
#define cm_upass fm.co.co_coupass
#define cm_grp fm.co.co_cogrp
#define cm_gpass fm.co.co_cogpass

#define	ut_type	dfs_hdr.dh_type
#define ut_stat	dfs_hdr.dh_stat
#define ut_error dfs_hdr.dh_error
#define ut_len	dfs_hdr.dh_len
#define ut_dwe dfs_hdr.dh_dwent
#define ut_ip fm.ut.ut_ino
#define ut_tacc	fm.ut.ut_ta
#define ut_tmod	fm.ut.ut_tmd

#define	co_type	dfs_hdr.dh_type
#define co_stat	dfs_hdr.dh_stat
#define co_error dfs_hdr.dh_error
#define co_len	dfs_hdr.dh_len
#define co_dwe	dfs_hdr.dh_dwent
#define co_ip fm.co.co_ino
#define co_uid fm.co.co_couid
#define co_gid fm.co.co_cogid
#define co_uuid fm.co.co_couuid
#define co_imode fm.co.co_coimode
#define co_user fm.co.co_couser
#define co_upass fm.co.co_coupass
#define co_grp fm.co.co_cogrp
#define co_gpass fm.co.co_cogpass
#define co_nuser fm.co.co_conuser
#define co_npassu fm.co.co_conpassu
#define co_ngrp fm.co.co_congrp
#define co_nxpassg fm.co.co_conxpassg

#define	s1_type	dfs_hdr.dh_type
#define s1_stat	dfs_hdr.dh_stat
#define s1_error dfs_hdr.dh_error
#define s1_len	dfs_hdr.dh_len
#define s1_dwe	dfs_hdr.dh_dwent
#define s1_ip	fm.s1.st1_st1ip
#define s1_statb	fm.s1.st1_stat
#define s1_dev	fm.s1.st1_stat.st_dev
#define s1_ino	fm.s1.st1_stat.st_ino
#define s1_mode	fm.s1.st1_stat.st_mode
#define s1_nlink	fm.s1.st1_stat.st_nlink
#define s1_uid	fm.s1.st1_stat.st_uid
#define s1_gid	fm.s1.st1_stat.st_gid
#define s1_rdev	fm.s1.st1_stat.st_rdev
#define s1_size	fm.s1.st1_stat.st_size
#define s1_atime	fm.s1.st1_stat.st_atime
#define s1_mtime	fm.s1.st1_stat.st_mtime
#define s1_ctime	fm.s1.st1_stat.st_ctime

#define	mkn_type	dfs_hdr.dh_type
#define mkn_stat	dfs_hdr.dh_stat
#define mkn_error dfs_hdr.dh_error
#define mkn_len	dfs_hdr.dh_len
#define mkn_dwe	dfs_hdr.dh_dwent
#define mkn_ip	fm.mknd.mkn_inode
#define mkn_rip	fm.mknd.mkn_rinode
#define mkn_rdev	fm.mknd.mkn_mrdev
#define mkn_uid	fm.mknd.mkn_muid
#define mkn_gid	fm.mknd.mkn_mgid
#define mkn_iflag	fm.mknd.mkn_miflag
#define mkn_fflag	fm.mknd.mkn_mfflag
#define mkn_mode	fm.mknd.mkn_mmode
#define mkn_imode	fm.mknd.mkn_mimode
#define mkn_cmask	fm.mknd.mkn_mcmask
#define mkn_offset	fm.mknd.mkn_moffset
#define mkn_dname	fm.mknd.mkn_mdname
#define mkn_dino	fm.mknd.mkn_mdino
#define mkn_user	fm.mknd.mkn_muser
#define mkn_upass	fm.mknd.mkn_mupass
#define mkn_grp		fm.mknd.mkn_mgrp
#define mkn_gpass	fm.mknd.mkn_mgpass

#define	unl_type	dfs_hdr.dh_type
#define unl_stat	dfs_hdr.dh_stat
#define unl_error dfs_hdr.dh_error
#define unl_len	dfs_hdr.dh_len
#define unl_dwe	dfs_hdr.dh_dwent
#define unl_ip	fm.unlk.unl_inode
#define unl_uid	fm.unlk.unl_uuid
#define unl_offset	fm.unlk.unl_uoffset
#define unl_pdir	fm.unlk.unl_updir
#define unl_dname	fm.unlk.unl_udname
#define unl_dino	fm.unlk.unl_udino
#define unl_user	fm.unlk.unl_uuser
#define unl_upass	fm.unlk.unl_uupass	

