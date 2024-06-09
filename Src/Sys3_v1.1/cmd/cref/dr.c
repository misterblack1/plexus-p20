#
# include	<stdio.h>
# include "mcons.h"
# include "ccmn.h"

int	(*acts[])() =	{0,
			coll,
			save,
			out,
			asym,
			asw,
			csym,
			csw,
			incl,
			decl,
			sk2,
			sk,
			tabs,
			semi
			};

char	*tmp[5]	= {"/usr/tmp/crt0a",
		"/usr/tmp/crt1a",
		"/usr/tmp/crt2a",
		"/usr/tmp/crt3a",
		"/usr/tmp/crt4a"
		};

char	*ignonl	= "/usr/lib/cref/aign";
char	*gtab	= "/usr/lib/cref/atab";

main(argc,argv)
	char	*argv[];
{
	auto	i,j,tm1,tm2,tm3;
	char	*fn,*av[8];

	if(argc < 2) {
		printf("Usage: cref [-acilonstux123] file1 ...\n");
		exit();
	}

	lbuf[4] = '\t';
	if(*argv[1] == '-') {
		j = flags(argv);
		argv += j;
		argc -= j;
	}
	if(argc == 2)	single = 1;


	init();

	i = 0;
	while(++i < argc) {
		curs[0] = '_';
		curs[1] = '\t';
		curs[4] = '\t';
		cursl = 2;

		if((ibuf1 = fopen(argv[i],"r")) == NULL) {
			printf("Can't open %s\n",argv[i]);
			dexit();
		}
		ibuf = ibuf1;

		curf[0] = '\t';
		curfl = 1;
		while((curf[curfl] = *argv[i]++) != 0 && curfl <= 14)
			if(curf[curfl++] == '/')	curfl = 1;
		curf[curfl++] = '\t';

		lno = 1;

		driver();

		fclose(ibuf1);
	}
	for(j = 0; j < 4;) {
		flsh(j,0);
		close(tp[j++]);
	}



	if(utmp)	exit();
	fn = "/bin/sort";
	av[0] = "sort";
	av[1] = "-i";
	av[3] = "-o";
	av[5] = 0;
	for(i = 0; i < 4; i++) {
		av[4] = av[2] = tmp[i];
		callsys(fn,av);
		if(utmp)	break;
	}


	if(usw) {
		fn = "/usr/lib/cref/upost";
		av[0] = "/usr/lib/cref/upost";
		i = 0;
	} else {
		fn = "/usr/lib/cref/crpost";
		av[0] = "/usr/lib/cref/crpost";
		av[1] = cross? "-4x": "-3";
		if(single)	av[1][1]--;
		i = 1;
	}
	j = -1;
	while(++j < 4) {
		av[++i] = tmp[j];
		if(utmp)	break;
	}
	av[++i] = 0;

	callsys(fn,av);


	dexit();
}

driver()
{
	auto	p;

	l = -1;
	while((c = line[++l] = getc(ibuf)) != EOF) {
		if(l >= 131) {
			printf("Line too long: %d %s\n",lno,curf);
			dexit();
		}

		if(c & 0200) {
			printf("Illegal character: %o line %d\n",c,lno);
			dexit();
		}

		if(fl) {
			if((*flag[fl])())
				continue;
		}


		if(p = tab[cs].cl[c])
			(*acts[p])();
		continue;
	}

}

init()
{
	short	b[3];
	auto	fi,i;
	extern	coll(),save(),out(),asym(),asw(),csym(),csw();
	extern	incl(),decl(),sk(),sk2();
	extern	dexit();


	xtab.hptr = xpsp;
	xtab.symt = xssp;
	xtab.hsiz = PTRX;
	xtab.ssiz = CHARX;
	xtab.nsym = 0;
	xtab.curb = 1;

	itab.hptr = ipsp;
	itab.symt = issp;
	itab.hsiz = PTRI;
	itab.ssiz = CHARI;
	itab.nsym = 0;
	itab.curb = 1;

	if((fi = open(gtab,0)) < 0) {
		printf("Cannot open grammar table; see lem\n");
		dexit();
	}

	i = -1;
	while(++i < NUMS)
		if(read(fi,tab[i].cl,256) < 256) {
			printf("Bad grammar table.\n");
			dexit();
		}

	close(fi);


	if((fi = open(ignonl,0)) < 0) {
		printf("Cannot open ignore/only file: %s\n",ignonl);
		dexit();
	}
	if(gtty(fi, b) != -1) {
		close(fi);
		compile();
		goto on;
	}
	if((read(fi,b,6) == 6) && (b[0] == (short)0100200)) {
		if(read(fi,itab.hptr,b[1]) < b[1]) {
			printf("Cannot read ignore/only file: %s\n",ignonl);
			dexit();
		}
		if(read(fi,itab.symt,b[2]) < b[2]) {
			printf("Cannot read ignore/only file: %s\n",ignonl);
			dexit();
		}
		close(fi);
	} else {
		close(fi);
		compile();
	}

on:
	if((signal(1,1) & 1) == 0)	signal(1,dexit);
	if((signal(2,1) & 1) == 0)	signal(2,dexit);
	if((signal(3,1) & 1) == 0)	signal(3,dexit);
	if(utmp == 0) {
		while((tp[4] = creat(tmp[4],0)) < 0)
			tmp[4][13]++;
		close(tp[4]);
		tmp[0][13] = tmp[4][13];
		tmp[1][13] = tmp[4][13];
		tmp[2][13] = tmp[4][13];
		tmp[3][13] = tmp[4][13];
		tp[0] = creat(tmp[0],CREATC);
		tp[1] = creat(tmp[1],CREATC);
		tp[2] = creat(tmp[2],CREATC);
		tp[3] = creat(tmp[3],CREATC);
	} else {
		if((tp[0] = creat(utmp,CREATC)) < 0) {
		printf("Can't create user's temp file.\n");
			exit();
		}
	}

	return;
}

error(a)
{
	printf("Error %d\n",a);
	dexit();
}

dexit()
{
	extern	nflush;

	if(tp[0] > 0 && utmp == 0) {
		unlink(tmp[0]);
		unlink(tmp[1]);
		unlink(tmp[2]);
		unlink(tmp[3]);
		unlink(tmp[4]);
	}
	exit();
}

callsys(f,v)
	char	f[],*v[];
{
	int	t,status,i;

	if((t = fork()) == 0) {
		execv(f,v);
		printf("Can't find %s\n",f);
		exit(1);
	} else {
		if(t == -1) {
			printf("Try again\n");
			return(1);
		}
	}

	while(t != wait(&status));
	if((t = (status & 0377)) != 0) {
		if(t != 2) {
			printf("Fatal error in %s\n",f);
			printf("t = %d\n",t);
		}
		dexit();
	}
	return((status>>8) & 0377);
}

flags(argv)
	char	*argv[];
{
	int	j,xx;
	char	*ap;

	j = 1;
	ap = argv[1];
	while(*++ap != '\0') {
		switch(*ap) {

			default:
				printf("Unrecognized flag: %c\n",*ap);
				dexit();

			case '1':	/* Symbol first */
				order = 1;
				continue;

			case '2':	/* Current file first */
				order = 2;
				continue;

			case '3':	/* Current symbol first */
				order = 3;
				continue;

			case 'a':	/* Assembler */
				cflag = 0;
				continue;

			case 'c':	/* C */
				gtab = "/usr/lib/cref/ctab";
				if(!xx)
					ignonl = "/usr/lib/cref/cign";
				cflag = 1;
				continue;

			case 'e':	/* English */
				gtab = "/usr/lib/cref/etab";
				if(!xx)
					ignonl = "/usr/lib/cref/eign";
				continue;

			case 'i':	/* Ignore file */
				if(!xx) {
					xx = 1;
					only = 0;
					ignonl = argv[++j];
				}
				continue;

			case 'l':	/* Line numbers in col. 3 */
				cross = 0;
				continue;

			case 'o':	/* Only file */
				if(!xx) {
					xx = 1;
					only = 1;
					ignonl = argv[++j];
				}
				continue;

			case 'n':	/* No context */
				nocont = 1;
				lbuf[4] = '\n';
				continue;

			case 's':	/* Symbols in col. 3 */
				cross = 1;
				continue;

			case 't':
				utmp = argv[++j];
				tmp[0] = argv[j];
				continue;

			case 'u':	/* Unique symbols only */
				usw = 1;
				continue;

			case 'x':	/* C externals */
				xsw = 1;
				gtab = "/usr/lib/cref/ctab";
				if(!xx)
					ignonl = "/usr/lib/cref/cign";
				cflag = 1;
				continue;
		}
	}
	return(j);
}


compile()
{
	char	buf[40],*b;
	int	i,v;

	ibuf1 = fopen(ignonl,"r");

	b = buf - 1;
	while((*++b = getc(ibuf1)) != -1) {
		if(*b == '\n') {
			*b = '\0';
			search(buf,b - buf,&itab,1);
			b = buf - 1;
		} else {
			if(*b == '\t') {
				v = 0;
				while((i = getc(ibuf1)) != -1) {
					if(i == '\n')	break;
					v = v*10 + (i - '0');
				}
				search(buf,b - buf,&itab,v);
				b = buf - 1;
			} else {
				if((b - buf) > 39) {
					printf("Ignore/only symbol too long");
					dexit();
				}
			}
		}
	}
	close(ibuf1);
	return;
}
