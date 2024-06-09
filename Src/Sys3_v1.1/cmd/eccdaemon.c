
/*
 * This program is used to pull the ecc single bit
 * error log entries out of kernal data space and
 * onto a log file. The entries in the file are as follows:
 *
 *	memory board address
 *	16 kbyte bank number 
 *	chip number
 *	date that error occured
 */

#include <sys/plexus.h>
#include <signal.h>
#include <sys/types.h>
#include <a.out.h>
#include <stdio.h>
#include <time.h>
#include <sys/ecc.h>
#include <sys/stat.h>

#define N -1  /* really no error, turned into "ecc turned off by kernel" */
#define	T -2  /* two errors, this should never get into the log */
#define M -3  /* three or more errors, should not appear either */
#define CX 16  /* check bits */
#define C0 17
#define C1 18
#define C2 19
#define C4 20
#define C8 21

/* the following table converts syndrome codes to bit in error
 * numbers.
 */

int stod[] = {
	 N, CX, C0,  T, C1,  T,  T,  M,
	C2,  T,  T,  1,  T,  M,  0,  T,
	C4,  T,  T,  2,  T,  3,  4,  T,
	 T,  5,  6,  T,  7,  T,  T,  M,
	C8,  T,  T,  8,  T,  9, 10,  T,
	 T, 11, 12,  T, 13,  T,  T,  M,
	 T, 14,  M,  T, 15,  T,  T,  M,
	 M,  T,  T,  M,  T,  M,  M,  N,
};

char *ctime();

char *sysmem = "/dev/kmem";
char *sysobj = UNIXNAME;
char *log = "/usr/adm/ecclog";
int kmemfn;
FILE *logfn;
struct stat stat;
struct ecc ecc[NUMECC];
struct nlist nl[] = {
	{"_ecc"},
	{""},
};

main(argc, argv)
int argc;
char **argv;
{
	register i;

	if ((kmemfn = open(sysmem, 0)) < 0) {
		fatal("couldn't open kernel memory file %s\n", sysmem);
	}
	if (argc == 2) {
		log = argv[1];
	}
	nlist(sysobj, nl);
	if (nl[0].n_type == 0) {
		fatal("couldn't open namelist file %s\n", sysobj);
	}
	if ((logfn = fopen(log, "a")) == NULL) {
		fatal("couldn't open log file %s\n", log);
	}
	fstat(fileno(logfn), &stat);
	lseek(kmemfn, (long) nl[0].n_value, 0);
	read(kmemfn, ecc, sizeof(ecc));
	for (i = 0; i < NUMECC; i++) {
		if (ecc[i].e_time > stat.st_mtime) {
			fprintf(logfn, "%d\t%d\t%d\t%s",
			       ((unsigned)ecc[i].e_bank) / 8,
			       ((unsigned)ecc[i].e_bank) % 8, 
			       stod[0x3f & ecc[i].e_syndrome],
			       ctime(&ecc[i].e_time));
		}
	}
	fclose(logfn);
	close(sysmem);
	exit(0);
}


fatal(message)
char *message;
{
	
	fprintf(stderr, "eccdaemon: %r", &message);
	exit(1);
}
