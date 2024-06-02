# include	"trek.h"
#include	<stdio.h>

/**
 **	on-board computer
 **/

CVNTAB	Cputab[]
{
	"r",			"ecord",
	"m",			"ove",
	"t",			"rajectory",
	"c",			"ourse",
	"s",			"core",
	"p",			"heff",
	"w",			"arpcost",
	"i",			"mpcost",
	"d",			"istresslist",
	"b",			"ase",
	"k",			"alculator",
	"x",			"",
	"y",			"",
	0
};

computer()
{
	char			c;
	int			ix, iy;
	register int		i, j;
	int			tqx, tqy, tsx, tsy;
	int			cost, course;
	float			dist, time;
	float			warpfac;
	double			d;
	int			dx,dy,b;
	int			x_axis;
	EVENT			*e;

	if (Damage[COMPUTER])
	{
		printf("Spock: Computer disabled.\n");
		return;
	}
	do{
	    if((i=getcodpar("Request: ", Cputab))<0) return;
	    switch(c = *(Cputab[i].abrev)) {

	    case 'r':			/* star chart */
		printf("Computer record of galaxy for all L.R. scans\n",
			 Status.shipname, Quadx, Quady);
		scanall(0);
		break;

	    case 't':			/* trajectory */
		if (Damage[SRSCAN])
		{
			printf("Spock: S.R. scanners damaged.\n");
			break;
		}
		if (Nkling <= 0)
		{
			printf("Spock: No Klingons in this quadrant.\n");
			break;
		}
		for (i = 0; i < Nkling; i++)
		{
			printf("Klingon at %d,%d", Kling[i].x, Kling[i].y);
			course=dokalc(Quadx, Quady, Kling[i].x, Kling[i].y, &dist);
			prkalc(course,dist);
		}
		break;

	    case 'c':			/* course */
	    case 'm':			/* move */
		if(readsep("/")) {
			tqx = Quadx;
			tqy = Quady;
		} else {
			if(getintpar("Quadrant", &ix)==0) break;
			readsep(",");
			if(getintpar("quadrant y-coordinate", &iy)==0) break;
			tqx = ix;
			tqy = iy;
			readsep("/");
		}
		if(getsect(&ix,&iy)==0)
			break;
		tsx = ix;
		tsy = iy;
		if(chkquad(tqx,tqy)) {
			course=dokalc(tqx, tqy, tsx, tsy, &dist);
			if(c=='m') {
				if(Status.cond!=DOCKED)
					warp(0,course,dist);
				else
					printf("Chekov: %s is docked.\n", Status.shipname);
			} else {
				printf("%d,%d/%d,%d to %d,%d/%d,%d",
					Quadx, Quady, Sectx, Secty, tqx, tqy, tsx, tsy);
				prkalc(course, dist);
			}
		}
		break;

	    case 's':			/* score */
		score();
		break;

	    case 'p':			/* phasers effective */
		if(getfltpar("Range", &dist)==0 ||
			chkdist(dist)==0) break;
		dist =* 10.0;
		cost = pow(0.90, dist) * 100 + 0.5;
		printf("Phasers are %d%% effective at that range.\n", cost);
		break;

	    case 'w':			/* warp cost */
		if(getfltpar("Distance", &dist)==0 ||
			chkdist(dist)==0 ||
			getfltpar("Warp factor", &warpfac)==0) break;
		cost = (dist + 0.05) * warpfac * warpfac * warpfac;
		if(Status.shldup)
			cost =* 2;
		time = Param.warptime * dist / (warpfac * warpfac);
		if(Status.cloaked)
			cost =+ Param.cloakenergy*time;
		printf("Distance %.2f cost %.2f stardates %d units",
			dist, time, cost);
		if(Status.shldup)
			printf(" (shields up)");
		if(Status.cloaked)
			printf(" (CLOAKED)");
		printf("\n");
		break;

	    case 'i':			/* impulse cost */
		if(getfltpar("Distance", &dist)==0) break;
		cost = 20 + 100 * dist;
		time = dist / 0.095;
		printf("Distance %.2f cost %.2f stardates %d units\n",
			dist, time, cost);
		break;


	    case 'd':			/* distresslist */
		j = 0;
		for (i = 0; i < MAXEVENTS; i++)
		{
			e = &Event[i];
			if ((e->systemname&E_NREPORT)==0)
				j =| report(e);
		}
		if (j==0)
			printf("No known distress calls are active.\n");
		break;

	    case 'b':			/* star base */
		if((b=findbase(&d))<0 && Damage[SRSCAN]) {
			printf("Spock: S.R. scanners damaged\n");
		} else {
			printf("nearest star base is ");
			if(b>=0) {
				dx=Base[b].x; dy=Base[b].y;
				printf("in quadrant %d,%d\n", dx, dy);
			} else	{
				dx=Starbase.x; dy=Starbase.y;
				printf("at sector %d,%d\n", dx, dy);
			}
		}
		break;

	    case 'x':			/* x scan */
	    case 'y':
		if(Damage[SRSCAN]) {
			printf("Spock: S.R. scanners damaged\n");
			break;
		}
		if(getintpar("coordinate",&ix)==0) break;
		if(ix<0 || ix>NSECTS) {
			printf("Out of range\n");
			break;
		}
		x_axis=(c=='x');
		printf("%d,%d ", (x_axis?ix:0), (x_axis?0:ix));
		for(i=0;i<NSECTS;i++) {
			printf(" %c", (x_axis ? Sect[ix][i] : Sect[i][ix]));
		}
		printf("  %d,%d\n", (x_axis?ix:NSECTS-1), (x_axis?NSECTS-1:ix));
		break;

	    case 'k':			/* calculator */
	    {
		FILE	*fp;
		char	buf[120];

		fp = popen("dc", "w");
		while(strcmp(fgets(&buf, sizeof buf, stdin), "off\n") != 0)
			fwrite(&buf, 1, strlen(buf), fp);
		pclose(fp);
	     }
		break;
	    }
	} while(readsep(";"));
}


dokalc(tqx, tqy, tsx, tsy, dist)
float *dist;
{
	double		dx, dy;
	float		quadsize;
	double		angle;
	int		course;

	quadsize = NSECTS;
	dx = (Quadx + Sectx / quadsize) - (tqx + tsx / quadsize);
	dy = (tqy + tsy / quadsize) - (Quady + Secty / quadsize);
	angle = atan2(dy, dx);
	if (angle < 0.0)
		angle =+ 6.283185307;
	course = angle * 57.29577951 + 0.5;
	dx = sqrt(dx*dx + dy*dy);
	*dist=dx;
	return(course);
}

prkalc(course,dist)
float dist;
{
	printf(": course %3d, distance %.3f\n", course, dist);
}

chkdist(d)
float d;
{
	register int b;

	if (b=(d < 0.0 || d > 15.0))
		printf("Distances must be positive.\n");
	return(!b);
}

chksect(sx,sy)
{
	register int b;

	if(b = (sx<0 || sy<0 || sx>=NSECTS || sy>=NSECTS))
		printf("%d,%d bad sector coordinates\n", sx, sy);
	return(!b);
}

chkquad(qx,qy)
{
	register int b;

	if(b = (qx<0 || qy<0 || qx>=NQUADS || qy>=NQUADS))
		printf("%d,%d bad quadrant coordinates\n", qx, qy);
	return(!b);
}

prsect(flg,i,j)
{
	register int	k, b, n;
	char		c;
	QUAD		*q;

	q = &Quad[i][j];
	if(flg) {
		q->scanned=((q->stars>=0)
				? (q->qkling * 100 + q->bases * 10 + q->stars)
				: 1000
			   );
	}
	n=q->scanned;
	if (n >= 1000)
		printf(((n==1000)?" ///":" .#."));
	else {
		if (n < 0)
			printf(" ...");
		else {
			k=n/100; b=(n =% 100)/10; n =% 10;
			if(i==Quadx && j==Quady)
				b=Status.ship;
			else	if(b)	b='#';
			c=(k?'0':' ');
			printf(" %c%c%c", (k?k+'0':' '), (b?b:c), n+'0');
		}
	}
}

getsect(ax,ay)
int	*ax, *ay;
{
	if(getintpar("Sector", ax)==0 ||
		(readsep(","), getintpar("sector y-coordinate", ay)==0) ||
		chksect(*ax,*ay)==0)
		return(0);
	return(1);
}

scanall(flg)
{
	register int	i, j;

	printf("  ");
	for (i = 0; i < NQUADS; i++)
		printf("-%d- ", i);
	printf("\n");
	for (i = 0; i < NQUADS; i++)
	{
		if(mkfault) break;
		printf("%d", i);
		for (j = 0; j < NQUADS; j++)
		{
			prsect(flg,i,j);
		}
		printf(" %d\n", i);
	}
	printf("  ");
	for (i = 0; i < NQUADS; i++)
		printf("-%d- ", i);
	printf("\n");
}
