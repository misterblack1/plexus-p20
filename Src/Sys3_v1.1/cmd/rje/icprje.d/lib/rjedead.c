/*
*	rjedead - creates file "dead" in directory (place)
*		containing message (msg).
*/

static char *dead = "dead";

rjedead(msg,place)
char *msg, *place;
{
	register int fd;

	chdir(place);
	unlink(dead);
	fd = creat(dead,0644);
	write(fd,msg,strlen(msg));
	exit(1);
}
