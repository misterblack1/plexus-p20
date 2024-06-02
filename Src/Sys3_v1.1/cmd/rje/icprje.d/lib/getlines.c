#include <rje.h>
#include <fcntl.h>

static int linzfd;
char *strchr();

/*
 * Read up to n entries from the lines
 * file.  Return -1 on error otherwise
 * return the number of entries read.
 */
getlines(lp,n)
register struct lines *lp;
register int n;
{
	register int count;

	if((linzfd=open(LINEFIL,O_RDONLY)) < 0)
		return(-1);
	count = 0;
	do {
		if(getlent(lp++) >= 0)
			count++;
	} while(--n);
	close(linzfd);
	return(count);
}

/*
 * Read the next entry from the
 * lines file.
 */
static
getlent(lp)
register struct lines *lp;
{
	register char *s;

	s = lp->l_buf;
	if(getstr(s,LBUFMAX,linzfd) < 0)
		return(-1);
	lp->l_host = s;
	if((s=strchr(s,'\t'))==0)
		return(-1);
	*s++ = '\0';
	lp->l_sys = s;
	if((s=strchr(s,'\t'))==0)
		return(-1);
	*s++ = '\0';
	lp->l_dir = s;
	if((s=strchr(s,'\t'))==0)
		return(-1);
	*s++ = '\0';
	lp->l_prefix = s;
	if((s=strchr(s,'\t'))==0)
		return(-1);
	*s++ = '\0';
	lp->l_dev = s;
	if((s=strchr(s,'\t'))==0)
		return(-1);
	*s++ = '\0';
	lp->l_peri = s;
	if((s=strchr(s,'\t'))==0)
		return(-1);
	*s++ = '\0';
	lp->l_parm = s;
	return(0);
}

/*
 *	Get lines from the file descriptor
 */
getstr(plc,max,fd)
char *plc;
int max, fd;
{
	register char *offset;
	register int red;
	long lseek();

	if((red = read(fd,plc,max)) <= 0)
		return(-1);
	offset = strchr(plc,'\n');
	*offset = '\0';
	lseek(fd,(long)-(&plc[red] - offset - 1),1);
}
