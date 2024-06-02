/*
 *	accton - calls syscall with super-user privileges
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#define	ROOT 0
#define	ERROR	-1
char pacct[]="/usr/adm/pacct";
struct stat buf;
int	uid;

main(argc,argv)
int argc;
char **argv;
{
	if(stat(pacct,&buf) == ERROR) {
		fprintf(stderr,"%s: cannot open %s, check turnacct\n",
							argv[0],pacct);
		exit(1);
	}
	if( (uid=getuid()) == ROOT || uid==buf.st_uid){
		if(setuid(ROOT) == ERROR) {
			fprintf(stderr,"%s: cannot setuid, ",argv[0]);
			fprintf(stderr,"check mode and owner\n");
			exit(1);
		}
		if (argc > 1) {
			if(acct(pacct) == ERROR) {
				fprintf(stderr,"%s: acctg not on\n",argv[0]);
				exit(1);
			}
		}
		else {
			if(acct(0) == ERROR) {
				fprintf(stderr,"%s: acctg not off\n",argv[0]);
				exit(1);
			}
		}
		exit(0);

	}
	fprintf(stderr,"permission denied - you must be root or adm\n");
	exit(1);
}
