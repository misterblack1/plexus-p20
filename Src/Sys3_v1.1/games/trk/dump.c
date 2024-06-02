# include	"trek.h"

/**
 **	dump game for later restart
 **/

#define	CHECK	7

dumpgame()
{
	register int	f, len;
	int		check;

	if((f=creat("trek.dump",0664))<0) {
		printf("Cannot create 'trek.dump'\n");
		return;
	}

	check=CHECK; write(f,&check,2);
	len=sbrk(0);
	if(write(f,0,len)!=len) {
		printf("Failed to write dump\n");
		return;
	}
	exit(0);
}

restart()
{
	register int	f, len;
	int		check;
	char		chkpass[PWDLEN];

	if((f=open("trek.dump",0))<0) {
		printf("Cannot open 'trek.dump'\n");
		return(0);
	}

	read(f,&check,2);
	len=sbrk(0);
	if(check!=CHECK || read(f,0,len)!=len) {
		printf("Cannot restart\n");
		exit(1);
	}
	getpasswd(chkpass);
	if (cf(chkpass, Game.passwd)) {
		printf("Incorrect password\n");
		exit(1);
	}
	unlink("trek.dump");
	return(1);
}
