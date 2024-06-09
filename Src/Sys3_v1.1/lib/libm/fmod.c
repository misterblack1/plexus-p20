double
fmod (a, b)
	double a, b;
{
	double d, modf();
	if (b == 0)
		return a;
	modf (a/b, &d);
	return a - d * b;
}
