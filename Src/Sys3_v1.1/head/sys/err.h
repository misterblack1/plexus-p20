/*
 * define space requirements
 */

#define	NESLOT	20

/*
 * structure of the err buffer area
 */
struct	err{
	int		e_nslot;		/* number of errslots */
	struct errhdr	**e_org;		/* orgion of buffer pool */
	struct errhdr	**e_nxt;		/* next slot to allocate */
	struct	errslot{
		int	slot[8];
	}	e_slot[NESLOT];			/* storage area */
	struct	map	e_map[(NESLOT+3)/2];	/* free space in map */
	struct errhdr	*e_ptrs[NESLOT];	/* pointer to logged errors */
};

extern	struct err err;

struct errhdr	*geteslot();
struct errhdr	*geterec();
