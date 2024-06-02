#include	"crash.h"
#include	"sys/inode.h"

prinode(c, md, all)
	register  int  c;
	int  md;
	int	all;
{
	struct  inode  ibuf;
	register  char  ch;
	register  int  i;

	if(c == -1)
		return;
	if(c > v.v_inode) {
		printf("%4d out of range\n", c);
		return;
	}
	lseek(kmem, (long)Inode->n_value + c * sizeof ibuf + delta, 0);
	if(read(kmem, &ibuf, sizeof ibuf) != sizeof ibuf) {
		printf("%4d read error on inode table\n");
		return;
	}
	if(!all && ibuf.i_count == 0)
		return;
	printf("%4d %3.3o %4.4o %5u %3d %4d %4d %4d %6ld",
		c, major(ibuf.i_dev), minor(ibuf.i_dev), ibuf.i_number,
		ibuf.i_count,
		ibuf.i_nlink, ibuf.i_uid, ibuf.i_gid, ibuf.i_size);
	switch(ibuf.i_mode & IFMT) {
	case IFDIR: ch = 'd'; break;
	case IFCHR: ch = 'c'; break;
	case IFBLK: ch = 'b'; break;
	case IFREG: ch = 'f'; break;
	default:    ch = '-'; break;
	}
	printf(" %c", ch);
	printf("%s%s%s%3o",
		ibuf.i_mode & ISUID ? "u" : "-",
		ibuf.i_mode & ISGID ? "g" : "-",
		ibuf.i_mode & ISVTX ? "v" : "-",
		ibuf.i_mode & 0777);
	printf(((ibuf.i_mode & IFMT) == IFCHR) ||
		((ibuf.i_mode & IFMT) == IFBLK) ?
		" %4.4o %4.4o" : "    -    -",
		major(ibuf.i_rdev), minor(ibuf.i_rdev));
	printf("%s%s%s%s%s%s%s\n",
		ibuf.i_flag & ILOCK ? " lck" : "",
		ibuf.i_flag & IUPD ? " upd" : "",
		ibuf.i_flag & IACC ? " acc" : "",
		ibuf.i_flag & IMOUNT ? " mnt" : "",
		ibuf.i_flag & IWANT ? " wnt" : "",
		ibuf.i_flag & ITEXT ? " txt" : "",
		ibuf.i_flag & ICHG ? " chg" : "");
	if(md == 1 && (((ibuf.i_mode & IFMT) == IFREG) ||
		((ibuf.i_mode & IFMT) == IFDIR))) {
		printf("     addr:");
		for(i = 0; i < NADDR; i++)
			printf(" %3ld", ibuf.i_addr[i]);
		printf("\n");
	}
}
