long
atol(p)
register char *p;
{
	long n;
	register int f;
	register int base;

	n = 0;
	f = 0;
	base = 10;
	for(;;p++) {
		switch(*p) {
		case ' ':
		case '\t':
			continue;
		case '0':
			base = 8;
			continue;
		case 'x':
		case 'X':
			if (base == 8) {
				base = 16;
				p++;
			}
			break;
		case '-':
			f++;
		case '+':
			p++;
		}
		break;
	}
	for(;;) {
		switch (*p) {
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = n*base + *p++ - '0';
			continue;
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			if (base <= 10)
				break;
			n = n*base + *p++ - 'a' + 10;
			continue;
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			if (base <= 10)
				break;
			n = n*base + *p++ - 'A' + 10;
			continue;
		}
		break;
	}
	return(f? -n: n);
}
