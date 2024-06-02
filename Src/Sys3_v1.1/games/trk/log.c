# include "trek.h"

#define	LOGFILE		"/usr/games/lib/treklog"

/**
 **	write out log
 **/

extern long	score();
extern CVNTAB	Skitab[];
extern CVNTAB	Lentab[];

char *reason[] {
	"win......",
	"time.....",
	"energy...",
	"destroyed",
	"barrier..",
	"nova.....",
	"supernova",
	"suffocate",
	"no help..",
	"too fast.",
	"burned...",
	"destruct.",
	"captured.",
	"no crew.."
};

int	stdout;

long writelog(n)
int	n;
{
	register int		f, uid;
	long			s;
	char			*date;

	s=score();
	if(Game.tourn && (f=open(LOGFILE,1))>=0) {
		uid=getuid()&0377;
		date=ctime(&inittime); date[10]=0;
		lseek(f,0L,2);
		stdout=f;
		printf("%d\t%s\t%D\t%s\t%s\t%s\t%.2f%c\t%d/%d\t%d\t%d\t%d\t%d\t%d\n",
			uid,
			date+4,
			s,
			reason[n],
			(Skitab[Game.skill-1].abrev),
			(Lentab[Game.length].abrev),
			Status.date-Initial.date,
			Status.ship,
			Game.gkillk, Game.gkillk+Status.kling,
			Game.helps,
			Game.killb,
			Game.kills,
			Game.killinhab,
			Game.deaths
			);
		stdout=1;
		close(f);
	}
	return(s);
}

