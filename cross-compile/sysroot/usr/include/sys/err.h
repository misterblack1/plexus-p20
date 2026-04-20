/* SID @(#)err.h	5.1 */
/* @(#)err.h	6.1 */
/*
 * structure of the err buffer area
 */
#define	NESLOT	20
#define	E_LOG	01
#define	E_SLP	02

struct err {
	int		e_nslot;		/* number of errslots */
	int		e_flag;			/* state flags */
	struct errhdr	**e_org;		/* origin of buffer pool */
	struct errhdr	**e_nxt;		/* next slot to allocate */
	struct errslot {
		int	slot[8];
	} e_slot[NESLOT];			/* storage area */
	struct map	e_map[(NESLOT+3)/2];	/* free space in map */
	struct errhdr	*e_ptrs[NESLOT];	/* pointer to logged errors */
};

extern struct err err;

struct errhdr	*geteslot();
struct errhdr	*geterec();

