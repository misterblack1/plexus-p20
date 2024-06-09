# include	"trek.h"

/**
 **	move under warp power
 **/

float newpos();

ram()
{
	register int	course;
	int		ix, iy;
	float		dist;

	if(getsect(&ix, &iy)==0)
		return;
	course=dokalc(Quadx,Quady,ix,iy,&dist);
	warp(1,course,dist);
}

move()
{
	int		course;
	float		dist;

	if(Status.cond==DOCKED) {
		printf("%s is docked.\n", Status.shipname);
		return;
	}
	if(getcodi(&course, &dist)==0)
		return;
	warp(0, course, dist);
}

warp(ramflg, c, d)
int		ramflg;
int		c;
double	d;
{
	int			course;
	char			evc;
	float			power, ftmp;
	float			dist;
	float			time;
	float			speed;
	double			frac;
	register int		percent;
	register int		i, sev;
	extern char		Snapshot[];

	if (Damage[WARP])
		if(Damage[WARP] == 1) {
			printf("Warp drive is disabled\n");
			return;
		}
		else if(Status.warp > (float)(Damage[WARP] - 1)) {
			printf("Scotty: We can only attain warp factor %d.\n",
				Damage[WARP] - 1);
			return;
		}
	course = c;
	dist = d;
	time = Param.warptime * dist / Status.warp2;
	power = (dist + 0.05) * Status.warp3;
	ftmp = Status.cloaked * Param.cloakenergy;
	ftmp *= time;
	power += ftmp;
	percent = 100 * power / Status.energy + 0.5;
	percent =* (Status.shldup+1);
	if (percent >= 85)
	{
		printf("Scotty: That would consume %d%% of our remaining energy.\n",
			percent);
		if (!getynpar("Are you sure that is wise: "))
			return;
		randmsg("Aye aye, sir!", 1);
	}
	percent = 100 * time / Status.time + 0.5;
	if (percent >= 85)
	{
		printf("Spock: That would take %d%% of our remaining time.\n",
			percent);
		if (!getynpar("Are you sure that is wise: "))
			return;
		randmsg("Aye aye, sir!", 1);
	}
	if (ranf(200) < 25 * (Status.warp - 6.0))
	{
		frac = franf();
		dist =* frac;
		time =* frac;
		damage(WARP, (frac + 1.0) * Status.warp * (franf() + 0.25) * 0.20, ranf(3) + 1);
		printf("Damage occurred to warp engines\n");
	}
	speed = Status.warp2 / Param.warptime;
	Move.time = newpos(ramflg, course, time, speed);
	dist = Move.time * speed;
	Status.energy =- dist * Status.warp3 * (Status.shldup + 1);
	if (Status.warp <= 9.0)
		return;
	printf("___ Speed exceeding warp nine ___\n");
	sleep(2);
	printf("Ship's safety systems malfunction\n");
	sleep(2);
	printf("Crew experiencing extreme sensory distortion\n");
	sleep(4);
	if (ranf(100) >= 50 * dist)
	{
		printf("Equilibrium restored -- all systems normal\n");
		return;
	}
	percent = ranf(100);
	if (percent < 90)
	{
		/* time warp */
		if (percent < 35 || !Game.snap)
		{
			time = (Status.warp - 8.0) * dist * (franf() + 1.0);
			Status.date =+ time;
			printf("Positive time portal entered -- it is now Stardate %.2f\n",
				Status.date);
			for (i = 0; i < MAXEVENTS; i++)
				if ((evc = Event[i].evcode) == E_FIXDV || evc == E_LRTB)
					reschedule(&Event[i], time);
		} else {
			time = Status.date;
			bmove(i=Snapshot, &Status.bases, 2);
			bmove(i =+ 2, &Status.date, 12);
			bmove(i =+ 12, &Quad, sizeof Quad);
			bmove(i =+ sizeof Quad, &Event, sizeof Event);
			bmove(i =+ sizeof Event, &Base, sizeof Base);
			bmove(i =+ sizeof Base, &Etc, sizeof Etc);
			printf("Negative time portal entered -- it is now Stardate %.2f\n",
				Status.date);
			for (i = 0; i < MAXEVENTS; i++)
				if (Event[i].evcode == E_FIXDV)
					reschedule(&Event[i], Status.date - time);
		}
	} else	if (percent < 95)
			lose(L_TOOFAST);
		else {
			printf("Equilibrium restored -- extreme damage occured to ship systems\n");
			for (i = 0; i < NDEV; i++) {
				switch(i) {
					case WARP:
					case IMPULSE:
					case PHASER:
					case TORPED:
					case CLOAK:
					case XPORTER:
						sev = ranf(3) + 1;
					default:
						sev = 1;
				}
				damage(i, (franf() + 1.0) * (Status.warp - 6) * Param.damfac[i] * 0.20, sev);
			}
			Status.shldup = 0;
			dcrept();
		}
}
