/*
	C library - ascii to floating
*/

#include <stdio.h>
#include <ctype.h>

char *Atof;
double
atof(p) register char *p; {
	register int c;
	register int exp;
	double rv;
	double fl, flexp;
#ifdef STOCKIII
	double big = 72057594037927936.;  /*2^56*/
#else
	double big = 9007199254740992.; /* 2^53 */
#endif
	double huge = 1.7e38;
	double ten21 = 1e21;
	int neg, negexp, eexp;

	Atof = p;
	neg = 1;
	if (*p == '-')
		++p, neg = -1;
	else if (*p=='+')
		++p;

	exp = 0;
	fl = 0;
	while(c = *p++) {
		if(!isdigit(c))
			break;
		if (fl<big)
			fl = 10*fl + (c-'0');
		else
			exp++;
	}

	if (c == '.') {
		while ((c = *p++), isdigit(c)) {
			if (fl<big) {
				fl = 10*fl + (c-'0');
				exp--;
			}
		}
	}

	negexp = 0;
	eexp = 0;
	if ((c == 'E') || (c == 'e')) {
		if ((c= *p++) == '+')
			;
		else if (c=='-')
			negexp = 1;
		else
			--p;

		while ((c = *p++), isdigit(c)) {
			eexp = 10*eexp+(c-'0');
		}
		if (negexp)
			eexp = -eexp;
		exp = exp + eexp;
	}

	negexp = 0;
	if (exp<0) {
		negexp = 1;
		exp = -exp;
	}

	if (exp == 0) {
		rv = neg*fl;
		goto ret;
	}
	if (exp>38) {
		if (negexp) {
			rv = 0;
			goto ret;
		} else {
			rv = neg*huge;
			goto ret;
		}
	}

	flexp = 1;
	while(exp>0) {
		flexp = 10*flexp;
		exp--;
		if (exp==21) {
			flexp *= ten21;
			exp -= 21;
		}
	}
	if (negexp) {
		fl /= flexp;
		if (flexp<1) 
			fl = 0;
	} else {
		fl *= flexp;
		if (flexp<1)
			fl = huge;
	}
	rv = neg*fl;
ret:
	Atof = --p;
	return rv;
}
