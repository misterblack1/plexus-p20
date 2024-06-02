# include	"trek.h"

/**
 **	set the warp factor
 **/

setwarp()
{
	float	warpfac;

	if(getfltpar("Warp factor", &warpfac)==0) return;
	if (warpfac < 0.0)
		return;
	if (warpfac < 1.0) {
		printf("Minimum warp speed is 1.0\n");
		return;
	}
	if (warpfac > 10.0) {
		printf("Maximum speed is warp 10.0\n");
		return;
	}
	if (warpfac > 6.0)
		printf("Damage to warp engines may occur above warp 6.0\n");
	Status.warp = warpfac;
	Status.warp2 = Status.warp * warpfac;
	Status.warp3 = Status.warp2 * warpfac;
}
