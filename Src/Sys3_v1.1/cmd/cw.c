#include <stdio.h>
#include <ctype.h>
#define EOL(c) (c == EOF || c == '\n')
#define GETCHAR() (incp ? ((buf[bufp] != '\0') ? buf[bufp++] : EOF) : getchar())
#define UNGETC(C, S) {if (incp) buf[--bufp] = C; else if (ungetc(C, S) == EOF) fprintf(stderr, "UNGETC ERROR");}
#define BUFSIZE 100

char	ldel[3];
char	rdel[3];
char	esc		= '\\';
char	font		= '3';
int	fflag		= 0;
int	tflag		= 1;
int	dflag		= 0;
int	bufp		= 0;

int	i, nl, xcp, incw, wcni, indel, counter, incp, incd, inacw, inacn;
char	v;
char	inp[3];
char	line[10];
char	buf[BUFSIZE];

struct s {
	char	*from;
	char	*to;
};

struct s	strans[] = {				/* troff comments */
	" ",	"\\0",
	"{",	"\\{",
	"}",	"\\}",
	"^",	"\\^",
	"\"",	"\\\"",
	"-",	"-",
	"e",	"\\(rg",
	"f",	"\\f",
	0
};

struct s	trans[] = {	/* special characters */
	"hy",	"\\(hy",	/* synonym for "-" in this font */
	"no",	"\\(ff",	/* EBCDIC "not" sign */
	"ct",	"\\(ct",	/* "cents" sign */
	"fm",	"\\(fm",	/* vertical single quote, apostrophe */
	"sq",	"\\(dg",	/* visible space symbol */
	"dg",	"\\(sq",	/* cap, hat, visible escape, control shift */
	"<-",	"\\(co",	/* left horizontal arrow */
	"->",	"\\(bu",	/* right horizontal arrow */
	"da",	"\\(em",	/* down arrow */
	"ua",	"\\(ua",	/* up arrow (APS & 202 only)*/
	"lh",	"\\(lh",	/* left up arrow, home arrow (APS & 202 only)*/
	0
};

char	*table[128]	={
	"\00",		/* nul */
	"\01",		/* soh */
	"\02",		/* stx */
	"\03",		/* etx */
	"\04",		/* eot */
	"\05",		/* enq */
	"\06",		/* ack */
	"\07",		/* bel */
	"\010",		/* bs */
	"\011",		/* ht */
	"\012",		/* nl */
	"\013",		/* vt */
	"\014",		/* np */
	"\015",		/* cr */
	"\016",		/* so */
	"\017",		/* si */
	"\020",		/* dle */
	"\021",		/* dc1 */
	"\022",		/* dc2 */
	"\023",		/* dc3 */
	"\024",		/* dc4 */
	"\025",		/* nak */
	"\026",		/* syn */
	"\027",		/* etb */
	"\030",		/* can */
	"\031",		/* em */
	"\032",		/* sub */
	"\033",		/* esc */
	"\034",		/* fs */
	"\035",		/* gs */
	"\036",		/* rs */
	"\037",		/* us */
	" ",		/* blank */
	"!",		/* ! */
	"\\(34",	/* " */
	"\\(de",	/* # */
	"$",		/* $ */
	"%",		/* % */
	"&",		/* & */
	"'",		/* ' */
	"(",		/* ( */
	")",		/* ) */
	"*",		/* * */
	"+",		/* + */
	",",		/* , */
	"-",		/* - */
	".",		/* . */
	"/",		/* / */
	"0",		/* 0 */
	"1",		/* 1 */
	"2",		/* 2 */
	"3",		/* 3 */
	"4",		/* 4 */
	"5",		/* 5 */
	"6",		/* 6 */
	"7",		/* 7 */
	"8",		/* 8 */
	"9",		/* 9 */
	":",		/* : */
	";",		/* ; */
	"\\(fi",	/* < */
	"=",		/* = */
	"\\(fl",	/* > */
	"?",		/* ? */
	"\\-",		/* @ */
	"A",		/* A */
	"B",		/* B */
	"C",		/* C */
	"D",		/* D */
	"E",		/* E */
	"F",		/* F */
	"G",		/* G */
	"H",		/* H */
	"I",		/* I */
	"J",		/* J */
	"K",		/* K */
	"L",		/* L */
	"M",		/* M */
	"N",		/* N */
	"O",		/* O */
	"P",		/* P */
	"Q",		/* Q */
	"R",		/* R */
	"S",		/* S */
	"T",		/* T */
	"U",		/* U */
	"V",		/* V */
	"W",		/* W */
	"X",		/* X */
	"Y",		/* Y */
	"Z",		/* Z */
	"[",		/* [ */
	"\\(rg",	/* \ */
	"]",		/* ] */
	"\\(14",	/* ^ */
	"\\(ru",	/* _ */
	"`",		/* ` */
	"a",		/* a */
	"b",		/* b */
	"c",		/* c */
	"d",		/* d */
	"e",		/* e */
	"f",		/* f */
	"g",		/* g */
	"h",		/* h */
	"i",		/* i */
	"j",		/* j */
	"k",		/* k */
	"l",		/* l */
	"m",		/* m */
	"n",		/* n */
	"o",		/* o */
	"p",		/* p */
	"q",		/* q */
	"r",		/* r */
	"s",		/* s */
	"t",		/* t */
	"u",		/* u */
	"v",		/* v */
	"w",		/* w */
	"x",		/* x */
	"y",		/* y */
	"z",		/* z */
	"\\(Fl",	/* { */
	"|",		/* | */
	"\\(Fi",	/* } */
	"\\(12",	/* ~ */
	"\177"		/* del */
};

main(argc, argv)
char **argv;
{
	while (--argc > 0 &&  ((*++argv)[0] == '-' || (*argv)[0] == '+'))
		option((*argv));
	printf(".fp %c CW\n", font);
	incw = indel = 0;
	if (dflag) {
		fprintf(stderr, "'\\\"  ldel = %s, rdel = %s, font = %c, tflag = %d\n", ldel, rdel, font, tflag);
		dflag = 0;
	}
	if (argc < 1)
		conv();
	else while (argc-- > 0) {
		if ((freopen(*argv++, "r", stdin)) == NULL) {
			fprintf(stderr, "cw: can't open %s\n", *--argv);
			exit(1);
		}
		conv();
	}
	exit(0);
}
							/* argument list */
option(s)						/* to cmd line, .CD, .CW & .CN */
char *s;
{
	if (!s[0]) return;
	switch(s[1]) {
	case 'l':
		if (s[2] != '\0') {
			strncpy(ldel, s+2, 2);
			ldel[2] = '\0';
		} else
			strcpy(ldel, "");
		break;
	case 'r':
		if (s[2] != '\0') {
			strncpy(rdel, s+2, 2);
			rdel[2] = '\0';
		} else
			strcpy(rdel, "");
		break;
	case 'f':
		if (incd || inacw || inacn)
			fflag = 1;
		if ((isdigit(s[2])) && (s[2] != '\0'))
			font = s[2];
		else
			font = '3';
		break;
	case 't':
		if (s[0] == '+')
			tflag = 1;
		else if (s[0] == '-')
			tflag = 0;
		break;
	case '\0':
		break;
	case 'd':
		dflag = 1;
		break;
	default:
		;
	}
}

conv()
{
	char	tabs[30];
	char	*t;
	int	c, c1;

	nl = 1;

	while ((c = GETCHAR()) != EOF)
		switch (c)
		{
		case '.':
		case '\'':
			if (nl != 1) {
				convc(c);
				break;
			}

			v = c;
			t = tabs;
			while ((c = GETCHAR()) == '\t' || c == ' ') {
				*t++ = c;
			}

			*t = NULL;
			if (c == '\n') {
				if (incw) {
					if (!tflag) {
						printf(".ft \\n(cE\n.ft \\n(cW\n");
						printf("%c%s%c.nr cW \\n(.f\n", v, tabs, c);
						printf(".ft P\n.nr cE \\n(.f\n.ft %c\n", font);
						v = '!';
						break;
					} else {
/*defeat ligatures */				printf("\\&");
						convc(v);
						printf("%s", tabs);
						convc(c);
						v = '!';
						break;
					}
				} else {
					printf("%c%s", v, tabs);
					UNGETC(c, stdin);
					v = '!';
					break;
				}
			}
			c1 = GETCHAR();
			if (c == 'C' && c1 == 'W') {	/* start constant-width */
				convcw(c, c1, tabs);

			} else if (c == 'C' && c1 == 'N') {	/* stop constant-width */
				convcn(c, c1, tabs);

			} else if (c == 'C' && c1 == 'D') {	/* change delimiters */
				convcd(c, c1, tabs);

			} else if ((c == 'C' && c1 == 'P' && !incw) || (c == 'P' && c1 == 'C' && !incw)) {
				convcp(c, c1, tabs);			/* change parameters */

			} else {			/* dot on beginning of line */
				if (convdot(c, c1, tabs) == 1)
					continue;
			}
			break;
		case '\\':
			convesc();
			break;
		default:
			convc(c);
			break;
		}
}
				/* start constant-width */
convcw(c, c1, tabs)
int	c, c1;
char	*tabs;
{
	inacw = 1;
	printf("%c%s%c%c", v, tabs, c, c1);
	v = '!';
	convarg(1);		/* print spaces */
	if (fflag) {
		printf(".fp %c CW\n", font);
		fflag = 0;
	}
	if (dflag) {
		fprintf(stderr, "'\\\"  ldel = %s, rdel = %s, font = %c, tflag = %d\n", ldel, rdel, font, tflag);
		dflag = 0;
	}
	if (incw == 0) {
		printf(".nr cW \\n(.f\n.ft P\n");
		printf(".nr cE \\n(.f\n.br\n.ft %c\n", font);
		printf(".cs %c 24\n", font);
		incw = 1;
	}
	inacw = 0;
}
				/* stop constant-width */
convcn(c, c1, tabs)
int	c, c1;
char	*tabs;
{
	inacn = 1;
	rdbuf();
	incp = 1;
	convarg(0);		/* no print spaces */
	incp = 0;
	if (fflag) {
		printf(".fp %c CW\n", font);
		fflag = 0;
	}
	if (incw) {
		printf(".br\n.cs %c\n", font);
		printf(".ft \\n(cE\n.ft \\n(cW\n");
		incw = 0;
	}
	printf("%c%s%c%c%s", v, tabs, c, c1, buf);
	v = '!';
	inacn = 0;
	if (dflag) {
		fprintf(stderr, "'\\\"  ldel = %s, rdel = %s, font = %c, tflag = %d\n", ldel, rdel, font, tflag);
		dflag = 0;
	}
}
				/* change delimiters */
convcd(c, c1, tabs)
int	c, c1;
char	*tabs;
{
	incd = 1;
	printf("%c%s%c%c", v, tabs, c, c1);
	v = '!';
	convarg(1);		/* print spaces */
	if (fflag) {
		printf(".fp %c CW\n", font);
		fflag = 0;
	}
	if (dflag) {
		fprintf(stderr, "'\\\"  ldel = %s, rdel = %s, font = %c, tflag = %d\n", ldel, rdel, font, tflag);
		dflag = 0;
	}
	incd = 0;
}
				/* change parameters */
convcp(c, c1, tabs)
int	c, c1;
char	*tabs;
{
	wcni = incw;
	printf("%c%s%c%c", v, tabs, c, c1);
	v = '!';
	if (c == 'P')
		counter = 0;
	else
		counter = 1;
	rdbuf();
	printf("%s", buf);
	incp = 1;
	do {
		while(isspace(c = GETCHAR()) && !EOL(c));
		++counter;
		if (counter&1) {
			incw = 0;
			if (counter != 1)
				printf("\\fP");
		} else {
			incw = 1;
			if (buf[0] != '\n')
				printf("\\&\\f%c", font);
		}
		while (!isspace(c) && c != EOF) {
			if (c == '"') {
				c = GETCHAR();
				top:
				while (!EOL(c) && c != '"') {
					if (c == esc) {
						convesc();
					} else if (c == ' ') {
						if (counter&1)
							printf(" ");
						else
							printf("\\ ");
					} else if (counter&1 && isdel(c, ldel)) {
						printf("%s", ldel);
					} else {
						convc(c);
					}
					c = GETCHAR();
				}
				if (c == '"') {
					c = GETCHAR();
					if (c == '"') {
						convc(c);
						c = GETCHAR();
						goto top;
					} else {
						UNGETC(c, stdin);
						break;
					}
				}
			} else {
				if (c == esc)
					convesc();
				else if (counter&1 && isdel(c, ldel))
					printf("%s", ldel);
				else
					convc(c);
				c = GETCHAR();
				if (c == '"') {
					convc(c);
					c = GETCHAR();
					if (c == '"') {
						convc(c);
						c = GETCHAR();
					}
				}
			}
		}
	} while (!EOL(c));
		if (incw && buf[0] != '\n')
			printf("\\fP");
	if (buf[0] != '\n') {
		convc(c);
	}
	incw = wcni;
	incp = 0;
	if (xcp) {
		printf(".nr cW \\n(.f\n.ft P \n");
		printf(".nr cE \\n(.f\n.ft %c\n", font);
		xcp = 0;
	}
}
				/* dot on beginning of line */
convdot(c, c1, tabs)
int	c, c1;
char	*tabs;
{
	if (incw) {
		if (!tflag) {
			printf(".ft \\n(cE\n.ft \\n(cW\n");
			if (c == 'C' && c1 == 'P' || c == 'P' && c1 == 'C') {
				xcp = 1;
				convcp(c, c1, tabs);
				return(1);
			}
			printf("%c%s", v, tabs);
			UNGETC(c1, stdin);
			ckdel(c);
			while (c = GETCHAR(), !EOL(c)) {
				if (c == esc && indel)
					convesc();
				else
					ckdel(c);
			}
				printf("\n.nr cW \\n(.f\n.ft P\n");
				printf(".nr cE \\n(.f\n.ft %c\n", font);
				v = '!';
				return(1);
		} else {
			UNGETC(c1, stdin);
/* defeat ligatures */	printf("\\&");
			convc(v);
			printf("%s", tabs);
			convc(c);
			v = '!';
			return(1);
		}
	} else {
		printf("%c%s", v, tabs);
		UNGETC(c1, stdin);
		ckdel(c);
	}
	while (c = GETCHAR(), !EOL(c)) {
		if (c == esc && indel)
			convesc();
		else
			ckdel(c);
	}
	if (c == '\n')
		UNGETC(c, stdin);
	return(0);
}
				/* character conversion */
convc(c)
int	c;
{
	if (incw && !indel) {
		if (!tflag && c == esc) {
			printf("\\");
		} else {
			printf("%s", table[c]);
			if (c == 'f')
				printf("\\&");		/* defeat ligatures */
			if (v == '.' || v == '\'') {
				if (!tflag) {
					printf(".nr cW \\n(.f\n.ft P\n");
					printf(".nr cE \\n(.f\n.ft %c\n", font);
					v = '!';
				}
			}
		}
	} else
		if (c == esc && indel)
			printf("%s", table[c]);
		else
			ckdel(c);
	if (c == '\n') {
		nl = 1;
		v = '!';
	} else
		nl = 0;
}
				/* check for delimiters */
ckdel(c)
int	c;
{
	if (!indel) {
		if (isdel(c, ldel)) {
			printf("\\&\\f%c", font);
			indel = 1;
		} else {
			printf("%c", c);
		}
	} else {
		if (isdel(c, rdel)) {
			printf("\\fP");
			indel = 0;
		} else {
			if (c == esc) {
				if (v == '.')
					return;
				else if (!tflag)
					printf("\\");
			} else if (c == ' ') {
				printf("\\ ");
			} else {
				printf("%s", table[c]);
				if (c == 'f')
	/* defeat ligatures */		printf("\\&");
			}
		}
	}
}
				/* esc conversion */
convesc()
{
	int	c;

	c = GETCHAR();
	if (c == EOF) {
		convc(esc);
		return;
	}
	if (c != '(') {
		inp[0] = c;
		inp[1] = '\0';
		if (!tflag) {
			for(i=0; strans[i].from != 0; i++) {
				if (!strcmp (strans[i].from, inp)) {
					if (incw || indel) {
						printf("%s", strans[i].to);
					} else if (!incw) {
						printf("\\%s", strans[i].from);
					}
					return;
				}
			}
		}
		convc(esc);
		convc(inp[0]);
		return;
	}
	inp[0] = GETCHAR();
	inp[1] = GETCHAR();
	inp[2] = '\0';
	if ((int)inp[0] == EOF) {
		convc(esc);
		convc(c);
		return;
	} else if ((int)inp[1] == EOF) {
		convc(esc);
		convc(c);
		convc(inp[0]);
		return;
	}
	for(i=0; trans[i].from != 0; i++) {
		if (!strcmp (trans[i].from, inp)) {
			if (incw || indel) {
				printf("%s", trans[i].to);
			} else if (!incw) {
				printf("\\(%s", trans[i].from);
			}
			return;
		}
	}
	convc(esc);
	convc(c);
	convc(inp[0]);
	convc(inp[1]);

}
				/* delimiter routine */
isdel(c, del)
int	c;						/* WARNING DO NOT MAKE MORE THEN */
char	*del;						/* ONE CALL TO LDEL OR RDEL */
{
	char *p;
	p=del;
	if (c != *p++) {				/* WARNING DELIMITERS CAN NOT */
		return(0);				/* BE LONGER THEN 2 CHARACTERS */
	}						/* WITH THIS CODE */
	if (*p == '\0') {
		return(1);
	}
	if ((c = GETCHAR()) != *p) {
		UNGETC(c, stdin);
		return(0);
	}
	return(1);
}
				/* read in argument line */
convarg(x)
int	x;
{
	int	c;

	do {
		while (isspace(c = GETCHAR()) && !EOL(c))
			if (x == 1) {
				putchar(c);
			} ;
		if (x == 1)
			putchar(c);
		i = 0;
		while (!isspace(c) && c != EOF) {
			line[i++] = c;
			c = GETCHAR();
			if (x == 1) {
				putchar(c);
			}
		}
		line[i] = '\0';
		option(line);
	} while (!EOL(c));
}
				/* read from buffer */
rdbuf()
{

	int	c;
	bufp = 0;
	do {
		buf[bufp++] = c = getchar();
	} while (!EOL(c));
	buf[bufp] = '\0';
	bufp = 0;
}
