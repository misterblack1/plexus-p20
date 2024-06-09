#include "max.h"
int bittab[2048], tcount;
char *cnames[]	=
	{"red", "yellow", "blue", "green",
	 "orange", "brown", "black", "white"};
int slots;
int colors;
int t1[MAXSLOTS];
int revtab [REVLEN], revcount;
/*
 *	This program plays the game of "Mastermind", described in
 *	the file "explain".
 *
 *	The portion of the program that guesses the human's
 *	configuration (play1()) works by storing all possible
 *	configurations in an array of bits (external bittab).
 *	The various routines in "marksubs" deal with this bit array,
 *	by "marking" and "unmarking" various configurations (turning
 *	the bit corresponsing to the configuration on and off) to
 *	indicate whether or not the configurations are possible given
 *	the human's previous answers. If the number of possible
 *	configurations (contained in "tcount") drops to zero, it means
 *	the human player is clowning around and the game restarts.
 *
 *	The table "cnames" defines the mapping from colors to the
 *	integers 0 through MAXCOLORS-1. Each configuration is stored
 *	in a single word, with each octal digit representing a
 *	color. Digits are occupied from the right (cf. "pack", "unpack").
 *
 *	When a person enters the number of hits and near-misses,
 *	the program increases the number of near-misses by the
 *	number of hits, because the program "imatch" returns
 *	a count which includes hits, also.
 */
main()
{
	int myscore, yourscore;

	if (yorn ("Do you want instructions? (y/n) "))
		instr();
	myscore = yourscore = 0;
	init();
	do {	myscore += play2();
		yourscore += play1();
		printf ("You %d, me %d.\n", yourscore, myscore);
	} while (yorn ("Play again? (y/n) "));
}
