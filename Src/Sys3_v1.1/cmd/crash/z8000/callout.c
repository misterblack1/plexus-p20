#include	"crash.h"
#include	"sys/callo.h"
#include	"sys/map.h"

#define	NMAP	50
#define	NCALL	50

prcallout()
{
	register  int  i;
	register  struct  nlist  *sp;
	struct  callo  callout[NCALL];
	extern long delta;

	lseek(kmem, (long)Callout->n_value + delta, 0);
	for(;;) {
		if(read(kmem, callout, sizeof callout) != sizeof callout) {
			printf("read error\n");
			return;
		}
		for(i = 0; i < NCALL; i ++) {
			if(callout[i].c_func == NULL)
				return;
			sp = search(callout[i].c_func, 042, 042);
			if(sp == NULL)
				printf("no match\n");
			else if(sp->n_name[0] == '_')
				printf("%-7.7s  %7.7o %5u\n", &sp->n_name[1],
					callout[i].c_arg, callout[i].c_time);
			else
				printf("%-8.8s %7.7o %5u\n", &sp->n_name[1],
					callout[i].c_arg, callout[i].c_time);
		}
	}
}

prmap(s)
	register  char  *s;
{
	struct	map	mbuf[NMAP];
	register  int  i;
	unsigned  free = 0, seg = 0;
	register  struct  nlist  *sp;
	extern long delta;

	printf("%s\n", s);
	if((sp = nmsrch(s)) == NULL) {
		printf("symbol not found\n");
		return;
	}
	printf("address  size\n");
	lseek(kmem, (long)sp->n_value + delta, 0);
	for(;;) {
		if(read(kmem, mbuf, sizeof mbuf) != sizeof mbuf) {
			printf("read error\n");
			return;
		}
		for(i = 0; i < NMAP; i++) {
			if(mbuf[i].m_size == 0) {
				printf("%u segments, %u units\n", seg, free);
				return;
			}
			printf("%7.7o %5u\n", mbuf[i].m_addr, mbuf[i].m_size);
			free += mbuf[i].m_size;
			seg++;
		}
	}
}
