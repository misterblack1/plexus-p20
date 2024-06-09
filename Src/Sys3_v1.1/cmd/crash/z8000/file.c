#include	"crash.h"
#include	"sys/file.h"
#include	"sys/inode.h"

prfile(c, all)
	register  int  c;
	int	all;
{
	struct	file	fbuf;

	if(c == -1)
		return;
	if(c >= v.v_file) {
		printf("%4d  out of range\n", c);
		return;
	}
	lseek(kmem, (long)File->n_value + c * sizeof fbuf + delta, 0);
	if(read(kmem, &fbuf, sizeof fbuf) != sizeof fbuf) {
		printf(" %4d read error on file table\n", c);
		return;
	}
	if(!all && fbuf.f_count == 0)
		return;
	printf(fbuf.f_inode ? "%4d  %3d   %3d  " : "%4d  %3d   -   ",
		 c, fbuf.f_count, ((unsigned)fbuf.f_inode - Inode->n_value)
		/ sizeof (struct inode) );
	printf("%s%s%s%s\n", fbuf.f_flag & FREAD ? " read" : "",
		fbuf.f_flag & FWRITE ? " write" : "",
		fbuf.f_flag & FNDELAY ? " ndelay" : "",
		fbuf.f_flag & FAPPEND ? " append" : "");
}
