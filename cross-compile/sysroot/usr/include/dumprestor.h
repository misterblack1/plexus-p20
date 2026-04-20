/*  */
/* @(#)dumprestor.h	5.1 4/22/86 */

#define NTREC   	10
#define MLEN    	16
#define MSIZ    	4096

#define TS_TAPE 	1
#define TS_INODE	2
#define TS_BITS 	3
#define TS_ADDR 	4
#define TS_END  	5
#define TS_CLRI 	6
#define MAGIC   	(unsigned short)60011
#define CHECKSUM	(short)84446
struct	spcl
{
	short	c_type;
	time_t	c_date;
	time_t	c_ddate;
	short	c_volume;
	daddr_t	c_tapea;
	ino_t	c_inumber;
	unsigned short	c_magic;
	short	c_checksum;
	struct	dinode	c_dinode;
	short	c_count;
	char	c_addr[BSIZE];
} spcl;

struct	idates
{
	char	id_name[16];
	char	id_incno;
	time_t	id_ddate;
};
