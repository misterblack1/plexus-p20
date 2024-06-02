/**
 **	get course and distance
 **/

getcodi(co, di)
int	*co;
float	*di;
{
	if(getintpar("Course", co)==0) return(0);
	fixco(co);
	readsep(",");
	if(getfltpar("Distance", di)==0) return(0);
	if(*di<0.0) {
		*di = -*di;
		*co =- 180;
		fixco(co);
	}
	return(1);
}

fixco(co)
int	*co;
{
	register int	neg;

	neg = *co<0;
	*co =% 360;
	if(neg)	*co =+ 360;
}
