extern float obotx;
extern float oboty;
extern float boty;
extern float botx;
extern float scalex;
extern float scaley;
xsc(xi){
	int xa;
	xa = (xi-obotx)*scalex+botx;
	return(xa);
}
ysc(yi){
	int ya;
	ya = (yi-oboty)*scaley+boty;
	return(ya);
}
