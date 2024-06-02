#include	"trek.h"

# define	NADMS	2

int	adms[NADMS] = {0, 92};


CVNTAB	Items[]
{
	"q",		"uad",
	"d",		"evice",
	"e",		"vent",
	"k",		"lingon",
	"i",		"nitial",
	"s",		"tatus",
	"g",		"ame",
	"m",		"ove",
	"p",		"aram",
	"systemname",	"",
	"things",	"",
	"sect",		"",
	"qx",	"",
	"qy",	"",
	"sx",	"",
	"sy",	"",
	"nkling",	"",
	"damage",	"",
	"base",		"",
	"starbase",	"",
	"cflg",		"",
	"rflg",		"",
	"rmsgs",	"",
	"violations",	"",
	"off",		"",
	"stop",		"",
	0
};

CVNTAB	Iquad[]
{
	"b",		"ases",
	"q",		"kling",
	"h",		"oles",
	"st",		"ars",
	"sys",		"temname",
	"sc",		"anned",
	"next",		"",
	0
};

CVNTAB	Idevice[]
{
	"n",		"ame",
	"p",		"erson",
	"next",		"",
	0
};

CVNTAB	Ievent[]
{
	"x",		"",
	"y",		"",
	"evc",		"ode",
	"evd",		"ata",
	"d",		"ate",
	"next",		"",
	0
};

CVNTAB	Ikling[]
{
	"x",		"",
	"y",		"",
	"p",		"ower",
	"d",		"ist",
	"a",		"vgdist",
	"next",		"",
	0
};

CVNTAB	Iinitial[]
{
	"ba",		"ses",
	"k",		"ling",
	"to",		"rped",
	"d",		"ate",
	"ti",		"me",
	"reso",		"urce",
	"e",		"nergy",
	"s",		"hield",
	"rese",		"rves",
	"c",		"rew",
	"br",		"igfree",
	"next",		"",
	0
};

CVNTAB	Istatus[]
{
	"ba",		"ses",
	"k",		"ling",
	"to",		"rped",
	"da",		"te",
	"ti",		"me",
	"reso",		"urce",
	"e",		"nergy",
	"shie",		"ld",
	"rese",		"rves",
	"cr",		"ew",
	"br",		"igfree",
	"shl",		"dup",
	"co",		"nd",
	"si",		"nsbad",
	"cloake",	"d",
	"warp",		"",
	"warp2",	"",
	"warp3",	"",
	"clockd",	"ate",
	"shipn",	"ame",
	"ship",		"",
	"di",		"stressed",
	"next",		"",
	0
};

CVNTAB	Igame[]
{
	"g",		"killk",
	"h",		"elps",
	"d",		"eaths",
	"n",		"egenbar",
	"killb",	"",
	"kills",	"",
	"sk",		"ill",
	"l",		"ength",
	"kille",	"d",
	"killi",	"nhab",
	"t",		"ourn",
	"p",		"asswd",
	"sn",		"ap",
	"c",		"aptives",
	"next",		"",
	0
};

CVNTAB	Imove[]
{
	"f",		"ree",
	"e",		"ndgame",
	"s",		"hldchg",
	"n",		"ewquad",
	"r",		"esting",
	"d",		"elta",
	"next",		"",
	0
};

CVNTAB	Iparam[]
{
	"damf",		"ac",
	"do",		"ckfac",
	"r",		"egenfac",
	"st",		"opengy",
	"sh",		"upengy",
	"k",		"lingpwr",
	"w",		"arptime",
	"p",		"hasfac",
	"movep",	"rob",
	"movef",	"ac",
	"e",		"ventdly",
	"n",		"avigcrud",
	"c",		"loakenergy",
	"damp",		"rob",
	"h",		"itfac",
	"next",		"",
	0
};

adm()
{
	register	uid, i;

	uid = getuid();
	for(i=0; i < NADMS; i++)
		if(adms[i] == uid)
			return(1);
	return(0);
}
maintain()
{
	register char	c;
	int	 i, n;
	int	 x, y;
	float	 f;

	if(!adm()) {
		printf("Cannot comply!\n");
		return;
	}
	printf("open for maintenance\n");

	while(1) {
		switch(getcodpar(">", Items)) {

		case 0:				/* quad */
			if((getintpar("x:", &x)) == 0)
				break;
			if((getintpar("y:", &y)) == 0)
				break;
			while((i = getcodpar(">>", Iquad)) != 6)
			switch(i) {
			case 0:
				printf("%d ?", Quad[x][y].bases);
				if((getintpar("", &n)) > 0)
					Quad[x][y].bases = n;
				continue;

			case 1:
				printf("%d ?", Quad[x][y].qkling);
				if((getintpar("", &n)) > 0)
					Quad[x][y].qkling = n;
				continue;

			case 2:
				printf("%d ?", Quad[x][y].holes);
				if((getintpar("", &n)) > 0)
					Quad[x][y].holes = n;
				continue;

			case 3:
				printf("%d ?", Quad[x][y].stars);
				if((getintpar("", &n)) > 0)
					Quad[x][y].stars = n;
				continue;

			case 4:
				printf("%d ?", Quad[x][y].systemname);
				if((getintpar("", &n)) > 0)
					Quad[x][y].systemname = n;
				continue;

			case 5:
				printf("%d ?", Quad[x][y].scanned);
				if((getintpar("", &n)) > 0)
					Quad[x][y].scanned = n;
				continue;

			default:
				break;
			}
			continue;

		case 1:				/* device */
			if((getintpar("#:", &x)) == 0)
				break;
			while((i = getcodpar(">>", Idevice)) != 2)
			switch(i) {
			case 0:
				printf("%s ?", Device[x].name);
				printf("no change\n");
				continue;

			case 1:
				printf("%s ?", Device[x].person);
				printf("no change\n");
				continue;

			default:
				break;
			}
			continue;

		case 2:				/* event */
			if((getintpar("#:", &x)) == 0)
				break;
			while((i = getcodpar(">>", Ievent)) != 5)
			switch(i) {
			case 0:
				printf("%d ?", Event[x].x);
				if((getintpar("", &n)) > 0)
					Event[x].x = n;
				continue;

			case 1:
				printf("%d ?", Event[x].y);
				if((getintpar("", &n)) > 0)
					Event[x].y = n;
				continue;

			case 2:
				printf("%d ?", Event[x].evcode);
				if((getintpar("", &n)) > 0)
					Event[x].evcode = n;
				continue;

			case 3:
				printf("%d ?", Event[x].evdata);
				if((getintpar("", &n)) > 0)
					Event[x].evdata = n;
				continue;

			case 4:
				printf("%f ?", Event[x].date);
				if((getfltpar("", &f)) > 0)
					Event[x].date = f;
				continue;

			default:
				break;
			}
			continue;

		case 3:				/* klingon */
			if((getintpar("#:", &x)) == 0)
				break;
			while((i = getcodpar(">>", Ikling)) != 5)
			switch(i) {
			case 0:
				printf("%d ?", Kling[x].x);
				if((getintpar("", &n)) > 0)
					Kling[x].x = n;
				continue;

			case 1:
				printf("%d ?", Kling[x].y);
				if((getintpar("", &n)) > 0)
					Kling[x].y = n;
				continue;

			case 2:
				printf("%d ?", Kling[x].power);
				if((getintpar("", &n)) > 0)
					Kling[x].power = n;
				continue;

			case 3:
				printf("%f ?", Kling[x].dist);
				if((getfltpar("", &f)) > 0)
					Kling[x].dist = f;
				continue;

			case 4:
				printf("%f ?", Kling[x].avgdist);
				if((getfltpar("", &f)) > 0)
					Kling[x].avgdist = f;
				continue;

			default:
				break;
			}
			continue;

		case 4:				/* initial */
			while((i = getcodpar(">>", Iinitial)) != 11)
			switch(i) {
			case 0:
				printf("%d ?", Initial.bases);
				if((getintpar("", &n)) > 0)
					Initial.bases = n;
				continue;

			case 1:
				printf("%d ?", Initial.kling);
				if((getintpar("", &n)) > 0)
					Initial.kling = n;
				continue;

			case 2:
				printf("%d ?", Initial.torped);
				if((getintpar("", &n)) > 0)
					Initial.torped = n;
				continue;

			case 3:
				printf("%f ?", Initial.date);
				if((getfltpar("", &f)) > 0)
					Initial.date = f;
				continue;

			case 4:
				printf("%f ?", Initial.time);
				if((getfltpar("", &f)) > 0)
					Initial.time = f;
				continue;

			case 5:
				printf("%f ?", Initial.resource);
				if((getfltpar("", &f)) > 0)
					Initial.resource = f;
				continue;

			case 6:
				printf("%d ?", Initial.energy);
				if((getintpar("", &n)) > 0)
					Initial.energy = n;
				continue;

			case 7:
				printf("%d ?", Initial.shield);
				if((getintpar("", &n)) > 0)
					Initial.shield = n;
				continue;

			case 8:
				printf("%f ?", Initial.reserves);
				if((getfltpar("", &f)) > 0)
					Initial.reserves = f;
				continue;

			case 9:
				printf("%d ?", Initial.crew);
				if((getintpar("", &n)) > 0)
					Initial.crew = n;
				continue;

			case 10:
				printf("%d ?", Initial.brigfree);
				if((getintpar("", &n)) > 0)
					Initial.brigfree = n;
				continue;

			default:
				break;
			}
			continue;

		case 5:				/* status */
			while((i = getcodpar(">>", Istatus)) != 22)
			switch(i) {
			case 0:
				printf("%d ?", Status.bases);
				if((getintpar("", &n)) > 0)
					Status.bases = n;
				continue;

			case 1:
				printf("%d ?", Status.kling);
				if((getintpar("", &n)) > 0)
					Status.kling = n;
				continue;

			case 2:
				printf("%d ?", Status.torped);
				if((getintpar("", &n)) > 0)
					Status.torped = n;
				continue;

			case 3:
				printf("%f ?", Status.date);
				if((getfltpar("", &f)) > 0)
					Status.date = f;
				continue;

			case 4:
				printf("%f ?", Status.time);
				if((getfltpar("", &f)) > 0)
					Status.time = f;
				continue;

			case 5:
				printf("%f ?", Status.resource);
				if((getfltpar("", &f)) > 0)
					Status.resource = f;
				continue;

			case 6:
				printf("%d ?", Status.energy);
				if((getintpar("", &n)) > 0)
					Status.energy = n;
				continue;

			case 7:
				printf("%d ?", Status.shield);
				if((getintpar("", &n)) > 0)
					Status.shield = n;
				continue;

			case 8:
				printf("%f ?", Status.reserves);
				if((getfltpar("", &f)) > 0)
					Status.reserves = f;
				continue;

			case 9:
				printf("%d ?", Status.crew);
				if((getintpar("", &n)) > 0)
					Status.crew = n;
				continue;

			case 10:
				printf("%d ?", Status.brigfree);
				if((getintpar("", &n)) > 0)
					Status.brigfree = n;
				continue;

			case 11:
				printf("%d ?", Status.shldup);
				if((getintpar("", &n)) > 0)
					Status.shldup = n;
				continue;

			case 12:
				printf("%d ?", Status.cond);
				if((getintpar("", &n)) > 0)
					Status.cond = n;
				continue;

			case 13:
				printf("%d ?", Status.sinsbad);
				if((getintpar("", &n)) > 0)
					Status.sinsbad = n;
				continue;

			case 14:
				printf("%d ?", Status.cloaked);
				if((getintpar("", &n)) > 0)
					Status.cloaked = n;
				continue;

			case 15:
				printf("%f ?", Status.warp);
				if((getfltpar("", &f)) > 0)
					Status.warp = f;
				continue;

			case 16:
				printf("%f ?", Status.warp2);
				if((getfltpar("", &f)) > 0)
					Status.warp2 = f;
				continue;

			case 17:
				printf("%f ?", Status.warp3);
				if((getfltpar("", &f)) > 0)
					Status.warp3 = f;
				continue;

			case 18:
				printf("%f ?", Status.cloakdate);
				if((getfltpar("", &f)) > 0)
					Status.cloakdate = f;
				continue;

			case 19:
				printf("%s ?", Status.shipname);
				printf("no change\n");
				continue;

			case 20:
				printf("%c ?", Status.ship);
				if(c = readchar())
					Status.ship = c;
				continue;

			case 21:
				printf("%d ?", Status.distressed);
				if((getintpar("", &n)) > 0)
					Status.distressed = n;
				continue;

			default:
				break;
			}
			continue;

		case 6:				/* game */
			while((i = getcodpar(">>", Igame)) != 14)
			switch(i) {
			case 0:
				printf("%d ?", Game.gkillk);
				if((getintpar("", &n)) > 0)
					Game.gkillk = n;
				continue;

			case 1:
				printf("%d ?", Game.helps);
				if((getintpar("", &n)) > 0)
					Game.helps = n;
				continue;

			case 2:
				printf("%d ?", Game.deaths);
				if((getintpar("", &n)) > 0)
					Game.deaths = n;
				continue;

			case 3:
				printf("%d ?", Game.negenbar);
				if((getintpar("", &n)) > 0)
					Game.negenbar = n;
				continue;

			case 4:
				printf("%d ?", Game.killb);
				if((getintpar("", &n)) > 0)
					Game.killb = n;
				continue;

			case 5:
				printf("%d ?", Game.kills);
				if((getintpar("", &n)) > 0)
					Game.kills = n;
				continue;

			case 6:
				printf("%d ?", Game.skill);
				if((getintpar("", &n)) > 0)
					Game.skill = n;
				continue;

			case 7:
				printf("%d ?", Game.length);
				if((getintpar("", &n)) > 0)
					Game.length = n;
				continue;

			case 8:
				printf("%d ?", Game.killed);
				if((getintpar("", &n)) > 0)
					Game.killed = n;
				continue;

			case 9:
				printf("%d ?", Game.killinhab);
				if((getintpar("", &n)) > 0)
					Game.killinhab = n;
				continue;

			case 10:
				printf("%d ?", Game.tourn);
				if((getintpar("", &n)) > 0)
					Game.tourn = n;
				continue;

			case 11:
				printf("%s ?", Game.passwd);
				printf("no change\n");
				continue;

			case 12:
				printf("%d ?", Game.snap);
				if((getintpar("", &n)) > 0)
					Game.snap = n;
				continue;

			case 13:
				printf("%d ?", Game.captives);
				if((getintpar("", &n)) > 0)
					Game.captives = n;
				continue;

			default:
				break;
			}
			continue;

		case 7:				/* move */
			while((i = getcodpar(">>", Imove)) != 6)
			switch(i) {
			case 0:
				printf("%d ?", Move.free);
				if((getintpar("", &n)) > 0)
					Move.free = n;
				continue;

			case 1:
				printf("%d ?", Move.endgame);
				if((getintpar("", &n)) > 0)
					Move.endgame = n;
				continue;

			case 2:
				printf("%d ?", Move.shldchg);
				if((getintpar("", &n)) > 0)
					Move.shldchg = n;
				continue;

			case 3:
				printf("%d ?", Move.newquad);
				if((getintpar("", &n)) > 0)
					Move.newquad = n;
				continue;

			case 4:
				printf("%d ?", Move.resting);
				if((getintpar("", &n)) > 0)
					Move.resting = n;
				continue;

			case 5:
				printf("%f ?", Move.delta);
				if((getfltpar("", &f)) > 0)
					Move.delta = f;
				continue;

			default:
				break;
			}
			continue;

		case 8:				/* param */
			while((i = getcodpar(">>", Iparam)) != 15)
			switch(i) {
			case 0:
				if(getintpar("#:", &x) == 0)
					continue;
				printf("%f ?", Param.damfac[x]);
				if(getfltpar("", &f) > 0)
					Param.damfac[x] = f;
				continue;

			case 1:
				printf("%f ?", Param.dockfac);
				if(getfltpar("", &f) > 0)
					Param.dockfac = f;
				continue;

			case 2:
				printf("%f ?", Param.regenfac);
				if(getfltpar("", &f) > 0)
					Param.regenfac = f;
				continue;

			case 3:
				printf("%d ?", Param.stopengy);
				if(getintpar("", &n) > 0)
					Param.stopengy = n;
				continue;

			case 4:
				printf("%d ?", Param.shupengy);
				if(getintpar("", &n) > 0)
					Param.shupengy = n;
				continue;

			case 5:
				printf("%d ?", Param.klingpwr);
				if(getintpar("", &n) > 0)
					Param.klingpwr = n;
				continue;

			case 6:
				printf("%d ?", Param.warptime);
				if(getintpar("", &n) > 0)
					Param.warptime = n;
				continue;

			case 7:
				printf("%f ?", Param.phasfac);
				if(getfltpar("", &f) > 0)
					Param.phasfac = f;
				continue;

			case 8:
				if(getintpar("#:", &x) == 0)
					continue;
				printf("%d ?", Param.moveprob[x]);
				if(getintpar("", &n) > 0)
					Param.moveprob[x] = n;
				continue;

			case 9:
				if(getintpar("#:", &x) == 0)
					continue;
				printf("%f ?", Param.movefac[x]);
				if(getfltpar("", &f) > 0)
					Param.movefac[x] = f;
				continue;

			case 10:
				if(getintpar("#:", &x) == 0)
					continue;
				printf("%f ?", Param.eventdly[x]);
				if(getfltpar("", &f) > 0)
					Param.eventdly[x] = f;
				continue;

			case 11:
				if(getintpar("#:", &x) == 0)
					continue;
				printf("%f ?", Param.navigcrud[x]);
				if(getfltpar("", &f) > 0)
					Param.navigcrud[x] = f;
				continue;

			case 12:
				printf("%d ?", Param.cloakenergy);
				if(getintpar("", &n) > 0)
					Param.cloakenergy = n;
				continue;

			case 13:
				if(getintpar("#:", &x) == 0)
					continue;
				printf("%f ?", Param.damprob[x]);
				if(getfltpar("", &f) > 0)
					Param.damprob[x] = f;
				continue;

			case 14:
				printf("%f ?", Param.hitfac);
				if(getfltpar("", &f) > 0)
					Param.hitfac = f;
				continue;

			default:
				break;
			}
			continue;

		case 9:				/* systemname */
			printf("not available\n");
			break;

		case 10:			/* things */
			printf("not available\n");
			break;

		case 11:			/* sect */
			printf("not available\n");
			break;

		case 12:			/* qx */
			printf("%d ?", Quadx);
			if(getintpar("", &n) > 0)
				Quadx = n;
			continue;

		case 13:			/* qy */
			printf("%d ?", Quady);
			if(getintpar("", &n) > 0)
				Quady = n;
			continue;

		case 14:			/* sx */
			printf("%d ?", Sectx);
			if(getintpar("", &n) > 0)
				Sectx = n;
			continue;

		case 15:			/* sy */
			printf("%d ?", Secty);
			if(getintpar("", &n) > 0)
				Secty = n;
			continue;

		case 16:			/* nkling */
			printf("%d ?", Nkling);
			if(getintpar("", &n) > 0)
				Nkling = n;
			continue;

		case 17:			/* damage */
			if(getintpar("#:", &x) == 0)
				break;
			printf("%d ?", Damage[x]);
			if(getintpar("", &n) > 0)
				Damage[x] = n;
			continue;

		case 18:			/* base */
			printf("not available\n");
			continue;

		case 19:			/* starbase */
			printf("not available\n");
			continue;

		case 20:			/* cflg */
			printf("%d ?", cflg);
			if(getintpar("", &n) > 0)
				cflg = n;
			continue;

		case 21:			/* rflg */
			printf("%d ?", rflg);
			if(getintpar("", &n) > 0)
				rflg = n;
			continue;

		case 22:			/* rmsgs */
			if(getintpar("#:", &x) == 0)
				break;
			printf("%d ?", rmsgs[x]);
			if(getintpar("", &n) > 0)
				rmsgs[x] = n;
			continue;

		case 23:			/* violations */
			printf("%d ?", violations);
			if(getintpar("", &n) > 0)
				violations = n;
			continue;

		case 24:
		case 25:
			printf("maintenance mode closed.\n");
			return;
		default:
			printf("Huh?\n");
			break;
		}
	}
}

update()
{
	if(!getynpar("Table update, Admiral? "))
		return;
	maintain();
}
