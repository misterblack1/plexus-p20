# include	"trek.h"

/**
 **	move under impulse power
 **/

float newpos();


impulse()
{
	int			course;
	register int		power, sev;
	float			dist, time, ispeed;
	register int		percent;

	if (Status.cond == DOCKED) {
		printf("Scotty: Sorry Captain, but we are still docked\n");
		randmsg("Has the strain been too much, sir?", 34);
		return;
	}
	ispeed = 0.095;
	if(sev = Damage[IMPULSE])
		if(sev == 1) {
			printf("Scotty: Impulse engines inoperable.\n");
			if(!Damage[WARP])
				randmsg("Perhaps the warp engines could be used?", 10);
			return;
		}
		else {
			printf("Scotty: We can only attain %d%% of normal \
			impulse speed.\n", (sev - 1) * 25);
			if(getynpar("Cancel that order: "))
				return;
			randmsg("Aye aye, sir!", 1);
			ispeed *= (sev - 1) * .25;
		}
	if (getcodi(&course, &dist)==0)
		return;
	power = 20 + 100 * dist;
	percent = 100 * power / Status.energy + 0.5;
	if (percent >= 85)
	{
		printf("Scotty: That would consume %d%% of our remaining energy.\n",
			percent);
		if (!getynpar("Are you sure that is wise: "))
			return;
		randmsg("Aye aye, sir!", 1);
	}
	time = dist / ispeed;
	percent = 100 * time / Status.time + 0.5;
	if (percent >= 85)
	{
		printf("Spock: That would take %d%% of our remaining time.\n",
			percent);
		if (!getynpar("Are you sure that is wise: "))
			return;
		randmsg("He's finally gone mad!", 15);
	}
	Move.time = newpos(0, course, time, ispeed);
	dist = Move.time * ispeed;
	Status.energy =- 20 + 100 * dist;
}
