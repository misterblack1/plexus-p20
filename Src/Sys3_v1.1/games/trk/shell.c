#include "trek.h"
/**
 **	call the shell
 **/

shell()
{
	int		i;
	register int	pid;
	register int	oldsig, oldqit;

	if (!(pid = fork()))
	{
		setuid(getuid());
		nice(0);
		signal(SIGINT, 0);
		execl("/bin/sh", "-", 0);
		printf("cannot execute /bin/sh");
		syserr();
	}
	oldsig=signal(2,1); oldqit=signal(3,1);
	while (wait(&i) != pid) ;
	printf("trek!\n");
	signal(2,oldsig); signal(3,oldqit);
}
