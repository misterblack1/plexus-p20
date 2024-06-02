#include <sys/param.h>
#include <sys/filsys.h>
#include <mnttab.h>
#define NMOUNT 20
struct mnttab mtab[NMOUNT], *mp;
main()
{

	int	rec, i, setcount;
	char	*cp,*cpfsys,inbuf[25];
	time_t	date;

	setcount = 0;
	if ((rec = creat("/etc/mnttab",0644)) < 0) {
		write(2,"creat error\n",12);
		exit(1);
	}
	time(&date);
	while(1) {
		for(cp = inbuf;;cp++) {
			if (read(0,cp,1) != 1) {
				write(rec,&mtab, sizeof mtab[0] * setcount);
				exit(1);
			}
			if (*cp == '\n') {
				*cp = '\0';
				break;
			}
		}
		setcount++;
		for(cp = inbuf; *cp && *cp != ' '; cp++);
		*cp++ = '\0';
		for(mp = mtab; mp <&mtab[NMOUNT]; mp++)  {
			if(mp->mt_dev[0] == 0) {
				strncpy(mp->mt_dev, inbuf, sizeof mp->mt_dev);
				strncpy(mp->mt_filsys, cp, sizeof mp->mt_filsys);
				mp->mt_time = date;
				mp->mt_ro_flg = 0;
				for(i = 0; i < 25; i++) {
					inbuf[i] = '\0';
				}
				break;
			}
		}
	}
}
