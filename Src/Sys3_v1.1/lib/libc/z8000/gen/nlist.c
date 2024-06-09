/*LINTLIBRARY*/
#include <a.out.h>
#define SPACE 100		/* number of symbols read at a time */

nlist(name, list)
char *name;
struct nlist *list;
{
	register struct nlist *p, *q;
	int f, n, m, i;
	long sa;
	struct exec buf;
	struct nlist space[SPACE];
	struct ovkhdr ovk;

	for(p = list; p->n_name[0]; p++) {
		p->n_type = 0;
		p->n_value = 0;
	}
	
	if((f = open(name, 0)) < 0)
		return(-1);
	read(f, (char *) &buf, sizeof(buf));
	if(BADMAG(buf)) {
		close(f);
		return(-1);
	}
	sa = buf.a_text + (long) buf.a_data;
	if ( buf.a_magic == A_MAGICO ) {
		sa += (long)sizeof(ovk);
		read( f, (char *) &ovk, sizeof(ovk));
		for ( i = 0; i < 7 && ovk.ovsize[i]; i++ )
			sa += (long)ovk.ovsize[i];
	}
#ifdef	vax
	sa += buf.a_trsize + buf.a_drsize;
#endif
#ifdef	z8000
	if(buf.a_flag != 1) sa *= 2;
#endif
	sa += sizeof(buf);
	lseek(f, sa, 0);
	n = buf.a_syms;

	while(n){
		m = sizeof(space);
		if(n < sizeof(space))
			m = n;
		read(f, (char *) space, m);
		n -= m;
		for(q = space; (m -= sizeof(struct nlist)) >= 0; q++) {
			for(p = list; p->n_name[0]; p++) {
				for(i=0; i < 8; i++)
					if(p->n_name[i] != q->n_name[i])
						goto cont;
				p->n_value = q->n_value;
				p->n_type = q->n_type;
				break;
		cont:		;
			}
		}
	}
	close(f);
	return(0);
}
