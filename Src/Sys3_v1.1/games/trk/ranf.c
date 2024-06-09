ranf(max)
int	max;
{
	double	d;
	register int	t;

	if (max <= 0)
		return (0);
	d = (float)rand()/32768.0;
	return((int)(d*(float)max));
}


double franf()
{
	double		t;
	t = rand();
	return (t / 32767.0);
}
