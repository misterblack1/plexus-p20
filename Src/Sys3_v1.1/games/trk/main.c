# include	"trek.h"

/**
 **		S T A R   T R E K
 **
 **	C version by Eric P. Allman 5/76 (U.C. Berkeley) with help
 **		from Jeff Poskanzer and Pete Rubinstein.
 **
 **	Please send comments, questions, and suggestions about this
 **		game to:
 **			Eric P. Allman
 **			Project INGRES
 **			Electronics Research Laboratory
 **			Cory Hall
 **			University of California
 **			Berkeley, California  94720
 **
 **	FORTRASH version by Kay R. Fisher (DEC) "and countless others".
 **	That was adapted from the "original BASIC program" (ha!) by
 **		Mike Mayfield (Centerline Engineering).
 **
 **	Additional inspiration taken from FORTRAN version by
 **		David Matuszek and Paul Reynolds which runs on the CDC
 **		7600 at Lawrence Berkeley Lab, maintained there by
 **		Andy Davidson.  This version is also available at LLL
 **		and at LMSC.  In all fairness, this version was the
 **		major inspiration for this version of the game (trans-
 **		lation:  I ripped off a whole lot of code).
 **
 **	Minor other input from the "Battelle Version 7A" by Joe Miller
 **		(Graphics Systems Group, Battelle-Columbus Labs) and
 **		Ross Pavlac (Systems Programmer, Battelle Memorial
 **		Institute).  That version was written in December '74
 **		and extensively modified June '75.  It was adapted
 **		from the FTN version by Ron Williams of CDC Sunnyvale,
 **		which was adapted from the Basic version distributed
 **		by DEC.  It also had "neat stuff swiped" from T. T.
 **		Terry and Jim Korp (University of Texas), Hicks (Penn
 **		U.), and Rick Maus (Georgia Tech).  Unfortunately, it
 **		was not as readable as it could have been and so the
 **		translation effort was severely hampered.  None the
 **		less, I got the idea of inhabited starsystems from this
 **		version.
 **
 **	The following features were added by:
 **
 **		Bert Olsson
 **		Bell Laboratories
 **		Murray Hill, N. J.
 **
 **	1. Severity levels for damaged devices:
 **		a. limited warp and impulse capabilities
 **		b. possible cloaking device failure
 **		c. limited phaser and torpedo accuracy
 **		d. `flakey' scanners (if they're damaged, they
 **			can tell lies!)
 **	2. Random messages and reminders.
 **	3. Regulation violations.
 **	4. Command line arguments:
 **		-c	suppress "Command"
 **		-r	suppress random messages
 **	5. Admiral privledges:
 **		a.	modification of internal tables
 **
 **	Permission is given for use, copying, and modification of
 **		all or part of this program and related documentation,
 **		provided that all reference to the authors are maintained.
 **/

fault()
{
	signal(SIGINT,fault);
	mkfault=1;
}

CVNTAB	Ngtab[]
{
	"n",	"o",
	"y",	"es",
	"pm",	"",
	"r",	"epeat",
	0
};

int	repflg;	/*Set when repeat games wanted*/
main(argc, argv)
int	argc;
char	**argv;
{
	int	two;

	time(&inittime);
	printf("\n   * * *   S T A R   T R E K   * * *\n\n");
	if(adm())
		printf("Welcome to the Enterprise, Admiral!\n");
	/* %%% begin */

	two=0;
	if(--argc > 0 && argv[1][0] == '-' && argv[1][1] != 0) {
		argv++;
		while(*++*argv) switch(**argv) {
			case 'c':
				cflg++;
				break;

			case 'r':
				rflg++;
				break;

			default:
				break;
		}
		argc--;
	}
	setexit();
	signal(2,0);
	if(two) {
sw:
		switch(getcodpar("Another game: ", Ngtab)) {
		    default:
			exit(0);
		    case 3:
			repflg++;
			break;
		    case 2:
			if(!adm())
				printf("cannot comply!\n");
			else {
				eventpr();
				scanall(1);
			}
			goto sw;

		    case 1:
			break;
		}
	}
	signal(SIGINT, fault);
	while(setup(two++) && play());
	printf("\n");
	exit(0);
}
