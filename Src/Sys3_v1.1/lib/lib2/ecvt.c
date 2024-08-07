/*LINTLIBRARY*/
/*
 *	ecvt converts to decimal
 *	the number of digits is specified by ndigit
 *	decpt is set to the position of the decimal point
 *	sign is set to 0 for positive, 1 for negative
 */

char	*cvt();

#define	NDIG	80
char*
ecvt(arg, ndigits, decpt, sign)
double arg;
int ndigits, *decpt, *sign;
{
	return(cvt(arg, ndigits, decpt, sign, 1));
}

char*
fcvt(arg, ndigits, decpt, sign)
double arg;
int ndigits, *decpt, *sign;
{
	return(cvt(arg, ndigits, decpt, sign, 0));
}

char XXbuf[NDIG];
static char*
cvt(arg, ndigits, decpt, sign, eflag)
double arg;
int ndigits, *decpt, *sign;
{
	register int r2;
	double fi, fj;
	register char *p, *p1;
	double modf();

	if (ndigits<0)
		ndigits = 0;
	if (ndigits>=NDIG-1)
		ndigits = NDIG-2;
	r2 = 0;
	*sign = 0;
	p = &XXbuf[0];
	if (arg<0) {
		*sign = 1;
		arg = -arg;
	}
	arg = modf(arg, &fi);
	p1 = &XXbuf[NDIG];
	/*
	 * Do integer part
	 */
	if (fi != 0) {
		p1 = &XXbuf[NDIG];
		while (fi != 0) {
			fj = modf(fi/10, &fi);
			*--p1 = (int)((fj+.03)*10) + '0';
			r2++;
		}
		while (p1 < &XXbuf[NDIG])
			*p++ = *p1++;
	} else if (arg > 0) {
		while ((fj = arg*10) < 1) {
			arg = fj;
			r2--;
		}
	}
	p1 = &XXbuf[ndigits];
	if (eflag==0)
		p1 += r2;
	*decpt = r2;
	if (p1 < &XXbuf[0]) {
		XXbuf[0] = '\0';
		return(XXbuf);
	}
	while (p<=p1 && p<&XXbuf[NDIG]) {
		arg *= 10;
		arg = modf(arg, &fj);
		*p++ = (int)fj + '0';
	}
	if (p1 >= &XXbuf[NDIG]) {
		XXbuf[NDIG-1] = '\0';
		return(XXbuf);
	}
	p = p1;
	*p1 += 5;
	while (*p1 > '9') {
		*p1 = '0';
		if (p1>XXbuf)
			++*--p1;
		else {
			*p1 = '1';
			(*decpt)++;
			if (eflag==0) {
				if (p>XXbuf)
					*p = '0';
				p++;
			}
		}
	}
	*p = '\0';
	return(XXbuf);
}
