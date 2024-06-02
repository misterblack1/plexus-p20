
#define ever (;;)

/* external function definitions */
extern double	franf();	/* floating random number function */
extern double	sqrt();		/* square root */
extern double	sin(), cos();	/* trig functions */
extern double	atan2();	/* fancy arc tangent function */
extern double	log();		/* log base e */
extern double	pow();		/* power function */
extern double	fabs();		/* absolute value function */
extern double	exp();		/* exponential function */

/*********************  GALAXY  **************************/

/* galactic parameters */
# define	NSECTS		10	/* dimensions of quadrant in sectors */
# define	NQUADS		10	/* dimension of galaxy in quadrants */
# define	NINHAB		32	/* number of quadrants which are inhabited */

#define	QUAD	struct quad
QUAD			/* definition for each quadrant */
{
	char	bases;		/* number of bases in this quadrant */
	char	qkling:4;	/* number of Klingons in this quadrant */
	char	holes:4;	/* number of black holes in this quadrant */
	char	stars;		/* number of stars in this quadrant */
	char	systemname;	/* starsystem name (see below) */
	int	scanned;	/* star chart entry (see below) */
};
/*  systemname conventions:
 *	1 -> NINHAB	index into Systemname table for live system.
 *	+ Q_GHOST	ghost starsystem.
 *	+ Q_DISTRESS	distressed starsystem.
 *			the index into the Distress table, which will
 *			have the starsystem name.
 *	0		dead or nonexistent starsystem
 *
 *  starchart ("scanned") conventions:
 *	0 -> 999	taken as is
 *	-1		not yet scanned ("...")
 *	1000		supernova ("///")
 *	1001		starbase + ??? (".1.")
 */

/* ascii names of systems */
extern char	*Systemname[NINHAB];

/* quadrant definition */
QUAD		Quad[NQUADS][NQUADS];

/* defines for sector map  (below) */
# define	NTHINGS		9

# define	EMPTY		0
# define	STAR		1
# define	KLINGON		2
# define	BLACKHOLE	3
# define	ENTERPRISE	4
# define	QUEENE		5
# define	INHABIT		6
# define	BASE		7

# define	EM		'.'
# define	ST		'*'
# define	KL		'K'
# define	BL		' '
# define	EN		'E'
# define	QU		'Q'
# define	IN		'O'
# define	BA		'B'

extern	char	things[NTHINGS];
/* current sector map */
char	Sect[NSECTS][NSECTS];

/* current position of Enterprise */
char	Quadx, Quady;		/* current quadrant */
char	Sectx, Secty;		/* current sector */

/************************ DEVICES ******************************/

# define	NDEV		16	/* max number of devices */
char	Damage[NDEV];			/* set if device damaged */

/* device tokens */
# define	WARP		0	/* warp engines */
# define	SRSCAN		1	/* short range scanners */
# define	LRSCAN		2	/* long range scanners */
# define	PHASER		3	/* phaser control */
# define	TORPED		4	/* photon torpedo control */
# define	IMPULSE		5	/* impulse engines */
# define	SHIELD		6	/* shield control */
# define	COMPUTER	7	/* on board computer */
# define	SSRADIO		8	/* subspace radio */
# define	LIFESUP		9	/* life support systems */
# define	SINS		10	/* Space Inertial Navigation System */
# define	CLOAK		11	/* cloaking device */
# define	XPORTER		12	/* transporter */
# define	SHUTTLE		13	/* shuttlecraft */

/* device names */
#define	DEVICE	struct device
DEVICE
{
	char	*name;		/* device name */
	char	*person;	/* the person who fixes it */
};

DEVICE		Device[NDEV];

/***************************  EVENTS  ****************************/

# define	NEVENTS		12	/* number of different event types */

# define	E_SNOVA		0	/* supernova occurs */
# define	E_LRTB		1	/* long range tractor beam */
# define	E_KATSB		2	/* Klingon attacks starbase */
# define	E_KDESB		3	/* Klingon destroys starbase */
# define	E_ISSUE		4	/* distress call is issued */
# define	E_ENSLV		5	/* Klingons enslave a quadrant */
# define	E_REPRO		6	/* a Klingon is reproduced */
# define	E_FIXDV		7	/* fix a device */
# define	E_ATTACK	8	/* Klingon attack during rest period */
# define	E_SNAP		9	/* take a snapshot for time warp */
# define	E_NREPORT	32	/* distress call not yet reported */
# define	Q_DISTRESS	64
# define	Q_GHOST		32
# define	Q_STARNAME	31

#define	EVENT	struct event
EVENT
{
	char	x, y;			/* coordinates */
	char	evcode;			/* event type */
	char	evdata;			/* starsystem name */
	float	date;			/* trap stardate */
};
/* systemname conventions:
 *	1 -> NINHAB	index into Systemname table for reported distress calls
 *	+ E_NREPORT	flag marking distress call not reported (SS radio out)
 */

# define	MAXEVENTS	25	/* max number of concurrently pending events */

EVENT		Event[MAXEVENTS];	/* dynamic event list; one entry per pending event */

/*****************************  KLINGONS  *******************************/

#define	KLINGONS	struct klingon
KLINGONS
{
	char	x, y;		/* coordinates */
	int	power;		/* power left */
	float	dist;		/* distance to Enterprise */
	float	avgdist;	/* average over this move */
};
# define	MAXKLQUAD	12	/* maximum klingons per quadrant */
KLINGONS	Kling[MAXKLQUAD];
int		Nkling;		/* number of Klingons in this sector */

/********************** MISCELLANEOUS ***************************/

/* condition codes */
# define	GREEN		0
# define	DOCKED		1
# define	YELLOW		2
# define	RED		3

/*
 *	note that much of the stuff in the following structs CAN NOT
 *	be moved around!!!!
 */

/* initial information */
struct
{
	char	bases;		/* number of starbases */
	char	kling;		/* number of klingons */
	char	torped;		/* photon torpedos */
	float	date;		/* stardate */
	float	time;		/* time left */
	float	resource;	/* Federation resources */
	int	energy;		/* starship's energy */
	int	shield;		/* energy in shields */
	float	reserves;	/* life support reserves */
	int	crew;		/* size of ship's complement */
	int	brigfree;	/* max possible number of captives */
}	Initial;

/* status information */
struct
{
	char	bases;		/* number of starbases */
	char	kling;		/* number of klingons */
	char	torped;		/* torpedoes */
	float	date;		/* stardate */
	float	time;		/* time left */
	float	resource;	/* Federation resources */
	int	energy;		/* starship's energy */
	int	shield;		/* energy in shields */
	float	reserves;	/* life support reserves */
	int	crew;		/* ship's complement */
	int	brigfree;	/* space left in brig */
	char	shldup;		/* shield up flag */
	char	cond;		/* condition code */
	char	sinsbad;	/* Space Inertial Navigation System condition */
	char	cloaked;	/* set if cloaking device on */
	float	warp;		/* warp factor */
	float	warp2;		/* warp factor squared */
	float	warp3;		/* warp factor cubed */
	float	cloakdate;	/* stardate we became cloaked */
	char	*shipname;	/* name of current starship */
	char	ship;		/* current starship */
	char	distressed;	/* number of currently distressed quadrants */
}	Status;

/* sinsbad is set if SINS is working but not calibrated */

/* game related information, mostly scoring */
#define PWDLEN 15
long	inittime;
struct
{
	int	gkillk;		/* number of klingons killed */
	int	helps;		/* number of help calls */
	int	deaths;		/* number of deaths onboard Enterprise */
	char	negenbar;	/* number of hits on negative energy barrier */
	char	killb;		/* number of starbases killed */
	int	kills;		/* number of stars killed */
	char	skill;		/* skill rating of player */
	char	length;		/* length of game */
	char	killed;		/* set if you were killed */
	char	killinhab;	/* number of inhabited starsystems killed */
	char	tourn;		/* set if a tournament game */
	char	passwd[PWDLEN];	/* game password */
	char	snap;		/* set if snapshot taken */
	int	captives;	/* total number of captives taken */
}	Game;

/* per move information */
struct
{
	char	free;		/* set if a move is free */
	char	endgame;	/* end of game flag */
	char	shldchg;	/* set if shields changed this move */
	char	newquad;	/* set if just entered this quadrant */
	char	resting;	/* set if this move is a rest */
	float	delta;		/* time used this move */
}	Move;

/* parametric information */
struct
{
	float	damfac[NDEV];	/* damage factor */
	float	dockfac;	/* docked repair time factor */
	float	regenfac;	/* regeneration factor */
	int	stopengy;	/* energy to do emergency stop */
	int	shupengy;	/* energy to put up shields */
	int	klingpwr;	/* Klingon initial power */
	int	warptime;	/* time chewer multiplier */
	float	phasfac;	/* Klingon phaser power eater factor */
	char	moveprob[6];	/* probability that a Klingon moves */
	float	movefac[6];	/* Klingon move distance multiplier */
	float	eventdly[NEVENTS];	/* event time multipliers */
	float	navigcrud[2];	/* navigation crudup factor */
	int	cloakenergy;	/* cloaking device energy per stardate */
	float	damprob[NDEV];	/* damage probability */
	float	hitfac;		/* Klingon attack factor */
}	Param;

/* Sum of damage probabilities must add to 1000 */

/* Other crap, mostly redundant stuff kept for efficiency reasons */
struct
{
	EVENT		*eventptr[NEVENTS];	/* pointer to event structs */
}	Etc;

/*
 *	eventptr is a pointer to the event[] entry of the last
 *	scheduled event of each type.  Zero if no such event scheduled.
 */

/* Klingon move indicies */
# define	KM_OB		0	/* Old quadrant, Before attack */
# define	KM_OA		1	/* Old quadrant, After attack */
# define	KM_EB		2	/* Enter quadrant, Before attack */
# define	KM_EA		3	/* Enter quadrant, After attack */
# define	KM_LB		4	/* Leave quadrant, Before attack */
# define	KM_LA		5	/* Leave quadrant, After attack */

/* you lose codes */
# define	L_NOTIME	1	/* ran out of time */
# define	L_NOENGY	2	/* ran out of energy */
# define	L_DSTRYD	3	/* destroyed by a Klingon */
# define	L_NEGENB	4	/* ran into the negative energy barrier */
# define	L_SUICID	5	/* destroyed in a nova */
# define	L_SNOVA		6	/* destroyed in a supernova */
# define	L_NOLIFE	7	/* life support died (so did you) */
# define	L_NOHELP	8	/* you could not be rematerialized */
# define	L_TOOFAST	9	/* pretty stupid going at warp 10 */
# define	L_STAR		10	/* ran into a star */
# define	L_DSTRCT	11	/* self destructed */
# define	L_CAPTURED	12	/* captured by Klingons */
# define	L_NOCREW	13	/* you ran out of crew */
#define		L_CHEAT		14	/* probably cheating */

# define	CVNTAB	struct cvntab
CVNTAB		/* used for getcodpar() paramater list */
{
	char	*abrev;
	char	*full;
};

#define	XY	struct xy
XY
{
	char	x, y;		/* coordinates */
};

/* starbase coordinates */
# define	MAXBASES	12	/* maximum number of starbases in galaxy */

#define	SIGINT	2
int	mkfault;			/* marks outstanding signal */

XY		Base[MAXBASES];		/* quad coords of starbases */
XY		Starbase;		/* starbase in current quadrant */

/*  distress calls  */
#define	MAXDISTR	8	/* maximum concurrent distress calls */

/* command line flags */
int	cflg;			/* suppress "command: " */
int	rflg;			/* suppress random messages */

/* flags to indicate if various random messages have been given */
# define	NMSGS		4

# define	ALONE		0
# define	BASES		1
# define	SRSC		2
# define	LRSC		3

char	rmsgs[NMSGS];

int	violations;		/* number of Federation regulation violations */
