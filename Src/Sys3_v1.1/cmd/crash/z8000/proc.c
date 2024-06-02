#include	"crash.h"
#include	"sys/proc.h"
#include	"sys/text.h"

prproc(c, md, run, all)
	int	c;
	int	md;
	int	run;
	int	all;
{
	char	ch, *cp;
	struct	user	ubuf;
	struct	proc	pbuf;
	extern	long	delta;
	int	i;

	if(c == -1)
		return;
	if(c > v.v_proc) {
		printf("%3d  out of range\n", c);
		return;
	}
	lseek(kmem, (long)Proc->n_value + (c * sizeof pbuf) + delta, 0);
	if(read(kmem, &pbuf, sizeof pbuf) != sizeof pbuf) {
		printf("%3d  read error on proc table\n", c);
		return;
	}
	if(!all && pbuf.p_stat == NULL)
		return;
	if(run && pbuf.p_stat != SRUN)
		return;
	switch(pbuf.p_stat) {
	case NULL:   ch = ' '; break;
	case SSLEEP: ch = 's'; break;
	case SWAIT:  ch = 'w'; break;
	case SRUN:   ch = 'r'; break;
	case SIDL:   ch = 'i'; break;
	case SZOMB:  ch = 'z'; break;
	case SSTOP:  ch = 't'; break;
	default:     ch = '?'; break;
	}
	printf("%3d %c", c, ch);
	printf(" %5u %5u %5u%5u  %2u %3u %6.6o %5u",
		pbuf.p_pid, pbuf.p_ppid, pbuf.p_pgrp,
		pbuf.p_uid, pbuf.p_pri & 0377, pbuf.p_cpu & 0377,
		pbuf.p_addr, pbuf.p_wchan);
	cp = "SWAPPED";
	if(pbuf.p_flag & SLOAD) {
		lseek(kmem, ctob((long)(unsigned)pbuf.p_addr)+delta-1048576, 0);
		if(read(kmem, &ubuf, sizeof ubuf) != sizeof ubuf)
			cp = "read err";
		else {
			if(c == 0)
				cp = "swapper";
			else
				cp = ubuf.u_comm;
			for(i = 0; i < 8 && cp[i]; i++) {
				if(cp[i] < 040 || cp[i] > 0176) {
					cp = "no print";
					break;
				}
			}
		}
	}
	if(pbuf.p_stat == NULL)
		cp = "";
	else if(pbuf.p_stat == SZOMB)
		cp = "ZOMBIE";
	printf(" %-8s", cp);
	printf("%s%s%s%s%s%s\n",
		pbuf.p_flag & SLOAD ? " load" : "",
		pbuf.p_flag & SSYS ? " sys" : "",
		pbuf.p_flag & SLOCK ? " lock" : "",
		pbuf.p_flag & SSWAP ? " swap" : "",
		pbuf.p_flag & STRC ? " trc" : "",
		pbuf.p_flag & SWTED ? " wted" : "");
	if(md == 0)
		return;
	if(pbuf.p_stat == SZOMB) {
		printf("     exit: %1.1o  user time: %ld  sys time: %ld\n",
		pbuf.xp_xstat, pbuf.xp_utime, pbuf.xp_stime);
		return;
	}
	printf(pbuf.p_textp ?
		"     sz:%u sig:%1.1o tm:%u nice:%d alm:%u q:%1.1o tx:%3d\n" :
		"     sz:%u sig:%1.1o tm:%u nice:%d alm:%u q:%1.1o \n",
		pbuf.p_size, pbuf.p_sig, pbuf.p_time, pbuf.p_nice,
		pbuf.p_clktim, pbuf.p_link,
		((unsigned)pbuf.p_textp - Text->n_value)/sizeof(struct text));
}
