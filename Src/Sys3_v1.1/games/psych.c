/*
	psych - draw pretty pictures on 4014

	compile as cc -s -n -O psych.c -l4014 -lm
		could use other plot-type library

*/
#include "stdio.h"

main() {
	int c;
	float g, fact, d, z, b;
	float x, y, limit = 0.25;
	int xx, yy;
	float sin(), cos();
	int i, first;
	int t[2];

	time(t);
	srand(t[1]);
  top:
	g = ((float)rand()/32767.) * 10;
	fact = ((float)rand()/32767.) * 20;
	erase();
	d = 0.1;
	z = 0;
	first = 0;
	for (i = 1; i <= 500; i++) {
		b = (z-110) / 1.57;
		z += g;
		g = (0-g) * fact;
		fact = 1 / fact;
		d *= 1.01;
		x = d * cos(b);
		y = d * sin(b);
		if (x*x > limit || y*y > limit)
			break;
		xx = (x+0.5) * 3120 + 500;
		yy = (y+0.5) * 3120;
		if (first == 0)
			move(xx, yy);
		cont(xx, yy);
		first = 1;
	}
	printf("\015");	/* out of plot mode */
	while ((c=getchar()) != '\n' && c!=EOF)
		;
	if(c==EOF) exit();
	goto top;
}
