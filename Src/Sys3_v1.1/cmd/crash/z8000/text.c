#include	"crash.h"
#include	"sys/text.h"
#include	"sys/inode.h"

prtext(c, all)
	int	c;
	int	all;
{
	struct	text	tbuf;
	char	x_flag;
	extern long int delta;

	if(c == -1)
		return;
	if(c >= v.v_text) {
		printf("%4d  out of range\n", c);
		return;
	}
	lseek(kmem, (long)Text->n_value + c * sizeof tbuf + delta, 0);
	if(read(kmem, &tbuf, sizeof tbuf) != sizeof tbuf) {
		printf("%4d  read error on text table\n", c);
		return;
	}
	if(!all && tbuf.x_iptr == NULL)
		return;
	printf("%4u  %5u  %3u  %3u %6.6o %5u %5u  ",
		c, tbuf.x_iptr ?
		((unsigned)tbuf.x_iptr - Inode->n_value) /
		sizeof(struct inode) : 0, tbuf.x_count, tbuf.x_ccount,
		tbuf.x_caddr, tbuf.x_daddr, tbuf.x_size);
	x_flag = tbuf.x_flag;
	printf("%s%s%s%s\n",
		x_flag & XWRIT ? "write" : "",
		x_flag & XLOAD ? "load" : "",
		x_flag & XLOCK ? "lock" : "",
		x_flag & XWANT ? "want" : "");
}
