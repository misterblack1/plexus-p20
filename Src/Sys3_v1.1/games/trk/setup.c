# include	"trek.h"

/**
 **	initialize game
 **/

CVNTAB	Lentab[]
{
	"restart",	"",
	"s",		"hort",
	"m",		"edium",
	"i",		"ntermediate",
	"l",		"ong",
	0
};

CVNTAB	Skitab[]
{
	"n",		"ovice",
	"f",		"air",
	"g",		"ood",
	"e",		"xpert",
	"c",		"ommodore",
	"i",		"mpossible",
	0
};

int	repflg;
double logdly();

setup(two)
{
	int			timevec[2];
	register int		i, j, n;
	float			f;
	int			d;
	int			klump;
	int			ix, iy;
	QUAD			*q;
	EVENT			*e;
	char			base_sq[NQUADS][NQUADS];

	time(timevec); srand(timevec[1]);
	if(repflg==0 && (Game.length=getcodpar("What length game: ", Lentab))<0) return(0);
	if(Game.length==0) {
		return(restart());
	}
	if(repflg==0 && (Game.skill=getcodpar("What skill game: ", Skitab)+1)<=0) return(0);
	Game.tourn = 1;
	if(two == 0)
		getpasswd(Game.passwd);
	Initial.bases = Status.bases = 7 - Game.skill;
	Initial.time = Status.time = 6.0 * Game.length + 2.0;
	i = Game.skill;
	j = Game.length;
	Initial.kling = Status.kling = i * j * 5;
	Initial.energy = Status.energy = 5000;
	Initial.torped = Status.torped = 10;
	Status.ship = things[ENTERPRISE];
	Status.shipname = "Enterprise";
	Initial.shield = Status.shield = 1500;
	Initial.resource = Status.resource = Initial.kling * Initial.time;
	Initial.reserves = Status.reserves = (6 - Game.skill) * 2.0;
	Initial.crew = Status.crew = 387;
	Initial.brigfree = Status.brigfree = 40;
	Status.shldup = 0;
	Status.cond = GREEN;
	Status.warp = 5.0;
	Status.warp2 = 25.0;
	Status.warp3 = 125.0;
	Status.sinsbad = 0;
	Status.cloaked = 0;
	Initial.date = Status.date = (ranf(20) + 20) * 100;
	f = Game.skill;
	for (i = 0; i < NDEV; i++)
		if (Device[i].name[0] == '*')
			Param.damfac[i] = 0;
		else
			Param.damfac[i] = log(f+0.5);
	/* these probabilities must sum to 1000 */
	Param.damprob[WARP] = 70;	/* warp drive		 7.0% */
	Param.damprob[SRSCAN] = 110;	/* short range scanners	11.0% */
	Param.damprob[LRSCAN] = 110;	/* long range scanners	11.0% */
	Param.damprob[PHASER] = 125;	/* phasers		12.5% */
	Param.damprob[TORPED] = 125;	/* photon torpedoes	12.5% */
	Param.damprob[IMPULSE] = 75;	/* impulse engines	 7.5% */
	Param.damprob[SHIELD] = 150;	/* shield control	15.0% */
	Param.damprob[COMPUTER] = 20;	/* computer		 2.0% */
	Param.damprob[SSRADIO] = 35;	/* subspace radio	 3.5% */
	Param.damprob[LIFESUP] = 30;	/* life support		 3.0% */
	Param.damprob[SINS] = 30;	/* navigation system	 3.0% */
	Param.damprob[CLOAK] = 40;	/* cloaking device	 4.0% */
	Param.damprob[XPORTER] = 80;	/* transporter		 8.0% */
	Param.dockfac = 0.5;
	Param.regenfac = (6 - Game.skill) * 0.02;
	Param.warptime = 10;
	Param.stopengy = 50;
	Param.shupengy = 40;
	i = Game.skill;
	Param.klingpwr = 100 + 150 * i;
	if (i >= 6)
		Param.klingpwr =+ 150;
	Param.phasfac = 0.8;
	Param.hitfac = 0.5;
	Param.moveprob[KM_OB] = 45;
	Param.movefac[KM_OB] = .09;
	Param.moveprob[KM_OA] = 40;
	Param.movefac[KM_OA] = -0.05;
	Param.moveprob[KM_EB] = 40;
	Param.movefac[KM_EB] = 0.075;
	Param.moveprob[KM_EA] = 25 + 5 * Game.skill;
	Param.movefac[KM_EA] = -0.06 * Game.skill;
	Param.moveprob[KM_LB] = 0;
	Param.movefac[KM_LB] = 0.0;
	Param.moveprob[KM_LA] = 10 + 10 * Game.skill;
	Param.movefac[KM_LA] = 0.25;
	Param.eventdly[E_SNOVA] = 0.5;
	Param.eventdly[E_LRTB] = 25.0;
	Param.eventdly[E_KATSB] = 1.0;
	Param.eventdly[E_KDESB] = 3.0;
	Param.eventdly[E_ISSUE] = 1.0;
	Param.eventdly[E_SNAP] = 0.5;
	Param.eventdly[E_ENSLV] = 0.5;
	Param.eventdly[E_REPRO] = 2.0;
	Param.navigcrud[0] = 1.50;
	Param.navigcrud[1] = 0.75;
	Param.cloakenergy = 1000;
	for (i = 0; i < MAXEVENTS; i++)
	{
		e = &Event[i];
		e->date = 1e38;
		e->evcode = -1;
	}
	schedule(E_SNOVA, logdly(E_SNOVA), 0, 0, 0);
	schedule(E_LRTB, logdly(E_LRTB), 0, 0, 0);
	schedule(E_KATSB, logdly(E_KATSB), 0, 0, 0);
	schedule(E_ISSUE, logdly(E_ISSUE), 0, 0, 0);
	schedule(E_SNAP, logdly(E_SNAP), 0, 0, 0);
	Quadx = ranf(NQUADS);
	Quady = ranf(NQUADS);
	Sectx = ranf(NSECTS);
	Secty = ranf(NSECTS);
	for (i = 0; i < NDEV; i++)
		Damage[i] = 0;
	for(i=0; i < NMSGS; i++)
		rmsgs[i] = 0;
	Game.gkillk = Game.kills = Game.killb = 0;
	Game.deaths = Game.negenbar = 0;
	Game.captives = 0;
	Game.killinhab = 0;
	Game.distresses = 0;
	Game.helps = 0;
	Game.killed = 0;
	Game.snap = 0;
	Move.endgame = 0;
	violations = 0;

	/* setup stars */
	for (i = 0; i < NQUADS; i++)
		for (j = 0; j < NQUADS; j++)
		{
			q = &Quad[i][j];
			q->qkling = q->bases = 0;
			q->scanned = -1;
			q->stars = ranf(9) + 1;
			q->holes = ranf(3-(q->stars/5));
			q->systemname = 0;
		}

	/* select inhabited starsystems */
	for (d = 1; d < NINHAB; d++)
	{
		do
		{
			i = ranf(NQUADS);
			j = ranf(NQUADS);
			q = &Quad[i][j];
		} while (q->systemname);
		q->systemname = d;
	}

	/* position starbases */
	d = (Initial.bases>4 ? 1 : 2);
	for (i = 0; i < NQUADS; i++)
		for (j = 0; j < NQUADS; j++)
			base_sq[i][j]=0;
	for (n = 0; n < Initial.bases; n++)
		while (1)
		{
			ix=ranf(NQUADS-2)+1;
			iy=ranf(NQUADS-2)+1;
			if(base_sq[ix][iy])
				continue;
			q = &Quad[ix][iy];
			for(i=ix-d; i<=ix+d; i++)
				for(j=iy-d; j<=iy+d; j++)
					if(i>=0 && j>=0 && i<NQUADS && j<NQUADS)
						base_sq[i][j]++;
			q->bases = 1;
			Base[n].x = ix;
			Base[n].y = iy;
			q->scanned = 1001;
			break;
		}

	/* position klingons */
	for (i = Initial.kling; i > 0; )
	{
		klump = ranf(4) + 1;
		if (klump > i)
			klump = i;
		while (1)
		{
			ix = ranf(NQUADS);
			iy = ranf(NQUADS);
			q = &Quad[ix][iy];
			if (q->qkling + klump > 9)
				continue;
			q->qkling =+ klump;
			i =- klump;
			break;
		}
	}

	if(adm())
		update();

	/* initialize this quadrant */
	printf("%d Klingons; it takes %d units to kill a Klingon.\n", Initial.kling, Param.klingpwr);
	printf("%d starbases at", Initial.bases);
	for (i = 0; i < Initial.bases; i++)
		printf(" %d,%d", Base[i].x, Base[i].y);
	printf("\n");
	Move.free = 0;
	initquad(0);
	scansys(1);
	attack(0);
	return(1);
}
