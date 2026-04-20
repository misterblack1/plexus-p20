/* signals.h for Plexus P/20 - AT&T UNIX System V Release 2 */
/* Signals: HUP INT QUIT ILL TRAP IOT EMT FPE KILL BUS SEGV SYS
            PIPE ALRM TERM USR1 USR2 CLD PWR VT */

#define SIGCOUNT       20

#ifdef GLOBALS

char *sigmsg[SIGCOUNT+2] = {
	"done",
	"hangup",
	"interrupt",
	"quit",
	"illegal instruction",
	"trace trap",
	"IOT instruction",
	"EMT instruction",
	"floating point exception",
	"killed",
	"bus error",
	"segmentation fault",
	"bad system call",
	"broken pipe",
	"SIGALRM",
	"terminated",
	"SIGUSR1",
	"SIGUSR2",
	"SIGCLD",
	"SIGPWR",
	"SIGVT",
	NULL
};

char *sigs[SIGCOUNT+4] = {
	"EXIT",
	"HUP",
	"INT",
	"QUIT",
	"ILL",
	"TRAP",
	"IOT",
	"EMT",
	"FPE",
	"KILL",
	"BUS",
	"SEGV",
	"SYS",
	"PIPE",
	"ALRM",
	"TERM",
	"USR1",
	"USR2",
	"CLD",
	"PWR",
	"VT",
	"ERR",
	"DEBUG",
	NULL
};

#else

extern char *sigs[SIGCOUNT+4],*sigmsg[SIGCOUNT+2];

#endif
