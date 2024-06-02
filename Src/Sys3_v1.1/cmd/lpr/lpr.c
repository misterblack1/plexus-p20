#include	<stdio.h>

/*
 *  lpr -- on line print to line printer
 */

#define	SPTYPE	LPR
#define FIRSTCHAR 'A'-1
#define	NTEMP	4
char	Xfname[NTEMP][25];
char	Xchar[NTEMP] = {'d', 't', 'c', 'l'};
char	*dfname = Xfname[0];
char	*tfname = Xfname[1];
char	*cfname = Xfname[2];
char	*lfname = Xfname[3];
char	*NAME;
int	BF = 'F';
#define	GCATSW	0
#define	MAXCOPY	512000L

#include	"spool.c"

int	troffit;
char	*fonts[4];
char	*plote;

main(agc, agv)
int agc;
char *agv[];
{
	char c;

	argc = agc;    argv = agv;
	setup();
#ifndef NMAIL
	wantmail = 1;		/*default is send mail.*/
#endif

	while (argc>1 && (arg = argv[1])[0]=='-') {
	    if((c = arg[1]) == 't'){
		troffit = 1;
		BF = 'T';
	    }else
		if(!comopt(c))
			switch (c) {

#ifndef STOCKIII
			case 'b':
				wantbanner = 0;
				break;
#endif
			case 'e':
				plote = getarg(c);
			case 'p':
				BF = 'P';
				break;

			case '4':
			case '3':
			case '2':
			case '1':
				fonts[c - '1'] = getarg(c);
				troffit = 1;
				BF = 'T';
				break;

			default:
				err("Unrecognized option: %s", arg);
				break;
			}
		argc--;
		argv++;
	}

	if(plote != 0){
		if(argc != 1)
			errout("Too many arguments to 'vpr -p -e'.");
		argc++;
		argv[1] = plote;
	}
	if(debug)
		tff = stdout;
	else
		if((tff = nfile(tfname)) == NULL)
			errout("Can't create %s.", tfname);
	if(ident())
		out();
	if(plote != 0)
		card('X', "-e");

	filargs();		/*process file arguments.*/

	if(wantmail)
		card('N', mailfile);
	if(debug)
		out();
	fclose(tff);
	if(nact) {
		dfname[INCHAR]++;
		if(link(tfname, dfname) < 0)
			errout("Cannot rename %s", tfname);
		unlink(tfname);
		execl(DAEMON, DAEMON, 0);
		execl(DAEMON2, DAEMON2, 0);
		err("Can't find %s.\nFiles left in spooling dir.", DAEMON);
		exit(1);
	}
	out();
}


archive()
{
}


nuact()
{
	int i;

	for (i = 0; i <= 3; i++)
		if (fonts[i])
			card('1' + i, fonts[i]);
}
