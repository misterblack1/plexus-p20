# include	"trek.h"

/**
 **	move under warp or impulse power
 **/

float newpos(ramflg, course, time, speed)
int	ramflg;
int	course;
float	time;
float	speed;
{
	double			angle;
	float			x, y, dx, dy;
	register int		ix, iy;
	double			bigger;
	int			n;
	register int		i;
	float			dist;
	float			sectsize;
	double			xn;
	float			evtime;

	sectsize = NSECTS;
	/* initialize delta factors for move */
	angle = course * 0.0174532925;
	if (Damage[SINS])
		angle =+ Param.navigcrud[1] * (franf() - 0.5);
	else
		if (Status.sinsbad)
			angle =+ Param.navigcrud[0] * (franf() - 0.5);
	dx = -cos(angle);
	dy = sin(angle);
	bigger = fabs(dx);
	dist = fabs(dy);
	if (dist > bigger)
		bigger = dist;
	dx =/ bigger;
	dy =/ bigger;

	/* check for LRTB */
	evtime = Etc.eventptr[E_LRTB]->date - Status.date;
	if (time > evtime && Nkling < 3)
	{
		time = evtime + 0.01;
	}
	dist = time * speed;

	/* move within quadrant */
	Sect[Sectx][Secty] = things[EMPTY];
	x = Sectx + 0.5;
	y = Secty + 0.5;
	xn = NSECTS * dist * bigger;
	n = xn + 0.5;
	Move.free = 0;

	for (i = 0; i < n; i++)
	{
		ix = (x =+ dx);
		iy = (y =+ dy);
		if (x < 0.0 || y < 0.0 || x >= sectsize || y >= sectsize)
		{
			/* enter new quadrant */
			ix = Quadx * NSECTS + Sectx + dx * xn + 0.5;
			iy = Quady * NSECTS + Secty + dy * xn + 0.5;
			Sectx = x;
			Secty = y;
			compkldist(0);
			Move.newquad = 2;
			attack(0);
			checkcond();
			Quadx = ix / NSECTS;
			Quady = iy / NSECTS;
			Sectx = ix % NSECTS;
			Secty = iy % NSECTS;
			if (ix < 0 || Quadx >= NQUADS || iy < 0 || Quady >= NQUADS)
				barrier("negative energy barrier");
			else	initquad(0);
			n = 0;
			break;
		}
		if (Sect[ix][iy] != things[EMPTY])
		{
			/* we just hit something */
			if (Damage[COMPUTER] || ramflg) {
				collide(ix, iy);
				ix = x - dx;
				iy = y - dy;
			} else if(Sect[ix][iy]==things[BLACKHOLE]) {
				printf("Spock: Captain, We are approaching a black hole\n");
				barrier("black hole");
				Status.resource =- Status.kling * (franf() + 0.5);
				n = 0;
			} else {
				ix = x - dx;
				iy = y - dy;
				printf("Computer reports navigation error; %s stopped at %d,%d\n",
					Status.shipname, ix, iy);
				Status.energy =- Param.stopengy * speed;
			}
			break;
		}
	}
	if (n > 0)
	{
		dx = Sectx - ix;
		dy = Secty - iy;
		dist = sqrt(dx * dx + dy * dy) / NSECTS;
		time = dist / speed;
		Sectx = ix;
		Secty = iy;
	}
	Sect[Sectx][Secty] = Status.ship;
	compkldist(0);
	return(time);
}

barrier(msg)
char	*msg;
{
	if (!Damage[COMPUTER])
	{
		Quadx = ranf(NQUADS);
		Quady = ranf(NQUADS);
		Sectx = ranf(NSECTS);
		Secty = ranf(NSECTS);
		printf("Computer applies full reverse power to avoid the %s\n", msg);
		printf(" A space warp was entered and you ended up in quadrant %d,%d\n",
			Quadx, Quady);
	}
	else
		lose(L_NEGENB);
	initquad(0);
}
