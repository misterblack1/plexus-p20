/* SID @(#)dir.h	5.1 */
/* @(#)dir.h	6.1 */
#ifndef	DIRSIZ
#define	DIRSIZ	14
#endif
struct	direct
{
	ino_t	d_ino;
	char	d_name[DIRSIZ];
};
