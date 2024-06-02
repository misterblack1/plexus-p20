#include	"crash.h"
#include	"sys/mount.h"
#include	"sys/inode.h"
#include	"sys/buf.h"
#include	"sys/filsys.h"
#include	"sys/ino.h"

prmount(c, all)
	int	c;
	int	all;
{
	struct	mount	mbuf;
	struct	filsys	fbuf;
	struct	buf	bbuf;
	extern long int delta;

	if(c == -1)
		return;
	if(c >= v.v_mount) {
		printf("%4d  out of range\n", c);
		return;
	}
	lseek(kmem, (long)Mount->n_value + c * sizeof mbuf + delta, 0);
	if(read(kmem, &mbuf, sizeof mbuf) != sizeof mbuf) {
		printf("%4d  read error on mount table\n", c);
		return;
	}
	if(!all && mbuf.m_flags == MFREE)
		return;
	printf("%4u  %3u  %3.3o  %5u  %3u  ",
		c, major(mbuf.m_dev), minor(mbuf.m_dev), mbuf.m_inodp ?
		((unsigned)mbuf.m_inodp - Inode->n_value) /
		sizeof(struct inode) : 0,
		mbuf.m_bufp ? ((unsigned)mbuf.m_bufp - Buf->n_value) /
		sizeof(struct buf) : 0);
	if(mbuf.m_flags == MFREE) {
		printf("\n");
		return;
	}
	lseek(kmem, (long)(unsigned)mbuf.m_bufp + delta, 0);
	if(read(kmem, &bbuf, sizeof bbuf) != sizeof bbuf) {
		printf("read error on mount buffer\n");
		return;
	}
	lseek(kmem, (bbuf.b_paddr&0xffff) + delta, 0);
	if(read(kmem, &fbuf, sizeof fbuf) != sizeof fbuf) {
		printf("read error on super block\n");
		return;
	}
	printf("%-6.6s  %-6.6s",fbuf.s_fname, fbuf.s_fpack);
	printf(" %6lu  %5u", fbuf.s_fsize, fbuf.s_isize * INOPB);
	printf(" %6lu  %5u\n", fbuf.s_tfree, fbuf.s_tinode);
}
