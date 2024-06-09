# include <stand.h>
# include <stdio.h>
# include <a.out.h>

# ifndef ROOT
# define ROOT	hp
# endif

# ifndef USR
# define USR	hp
# endif

# define KERNEL	0
# define USER	1

extern	argc;
char	*argv [32];
char	argbuf [256];

# ifdef vax
int	(*entry) ();
# endif

unsigned sread ();

int	_nullsys ();
int	_hpstrategy ();
# ifdef pdp11
int	_rpstrategy ();
int	_rkstrategy ();
# endif

struct devsw _devsw [] = {
/* 0: hp */	_hpstrategy,	_nullsys,	_nullsys,
# ifdef pdp11
/* 1: rp */	_rpstrategy,	_nullsys,	_nullsys,
/* 2: rk */	_rkstrategy,	_nullsys,	_nullsys,
# endif
};

int	_devcnt = sizeof (_devsw) / sizeof (*_devsw);

char	hp [] = "/dev/hp0";
# ifdef pdp11
char	rp [] = "/dev/rp0";
char	rk [] = "/dev/rk0";
# endif

char	devname [][5] = {
	"rp04",
# ifdef pdp11
	"rp03",
	"rk05",
# endif
};

# ifdef vax
# define PAGE	0x1ff
# define HBSS	0x4f800
# define SET2	"       set [disk {/|/usr} rp04]\n"
# endif

# ifdef pdp11
# define PAGE	017777
# define HBSS	0160000
# define SET2	"       set [disk {/|/usr} {rp04|rp03|rk05}]\n"
# endif

main () {
	extern char edata, end;

	clrseg (&edata, (unsigned) (&end - &edata), USER);

# ifdef pdp11
	segflag = 2;
# endif

	if (MKNOD (hp, 0, 0, (daddr_t) 0) < 0)
		perror (hp);

	hp [7] = '1';
	if (MKNOD (hp, 0, 0, (daddr_t) 18392) < 0)
		perror (hp);

# ifdef pdp11
	if (MKNOD (rp, 1, 0, (daddr_t) 0) < 0)
		perror (rp);

	rp [7] = '1';
	if (MKNOD (rp, 1, 0, (daddr_t) 10000) < 0)
		perror (rp);

	if (MKNOD (rk, 2, 0, (daddr_t) 0) < 0)
		perror (rk);

	rk [7] = '1';
	if (MKNOD (rk, 2, 0, (daddr_t) 0) < 0)
		perror (rk);
# endif

	hp [7] = '0';
	if (mount (ROOT, "") < 0)
		perror (hp);

	hp [7] = '1';
	if (mount (USR, "/usr") < 0)
		perror (hp);

	_prs ("\n");
}

xcom () {
	register ac;

	while (load () < 0);
# ifdef pdp11
	for (ac = 0; ac < argc; ac++)
		argv [ac] += (char *) 0157400 - argbuf;
	movc3 (0400, argbuf, (char *) 0157400, USER);
	movc3 (0100, (char *) argv, (char *) 0157300, USER);
# endif
}

load () {
	register unsigned count;
	register char *corep;
	register in;
	struct exec hdr;
	extern char *_cdir, *strcpy ();

	getargv ();
	if (argc == 0)
		return (-1);
	if (strcmp (argv [0], "set") == 0) {
		set ();
		return (-1);
	}
	if (strcmp (argv [0], "cd") == 0) {
		switch (argc) {
		case 1:
			ppath (_cdir, "\n");
			return (-1);
		case 2:
			chdir (argv [1]);
			return (-1);
		}
		_prs ("Usage: cd [directory]\n");
		return (-1);
	}
	if ((in = open (argv [0], 0)) < 0) {
		_prs (argv [0]);
		_prs (": not found\n");
		return (-1);
	}
	if (read (in, (char *) &hdr, sizeof (hdr)) != sizeof (hdr)
	    || (hdr.a_magic != A_MAGIC1
# ifdef pdp11
	    && hdr.a_magic != A_MAGIC3
# endif
	    && hdr.a_magic != A_MAGIC2)) {
		movc3 (sizeof (hdr), (char *) &hdr, (char *) 0, USER);
		if ((corep = (char *) sread (in, (char *) sizeof (hdr), 0xffff)) < 0) {
		error:
			_prs (argv [0]);
			_prs (": read error\n");
			return (-1);
		}
		corep += sizeof (hdr);
		hdr.a_bss = HBSS - (unsigned) corep;
# ifdef vax
		entry = 0;
# endif
	} else {
		switch (hdr.a_magic) {
		case A_MAGIC1:
		case A_MAGIC2:
			count = hdr.a_text + hdr.a_data;
			if (sread (in, (char *) 0, count) != count)
				goto error;
			corep = (char *) hdr.a_text;
			if (hdr.a_magic == A_MAGIC2 && hdr.a_data) {
				corep = (char *) (((int) corep + PAGE) & ~PAGE);
				movc3 (hdr.a_data, (char *) hdr.a_text, corep, KERNEL);
			}
			break;

# ifdef pdp11
		case A_MAGIC3:
			lseek (in, (off_t) (hdr.a_text + sizeof (hdr)), 0);
			count = hdr.a_data;
			if (sread (in, (char *) 0, count) != count)
				goto error;
			count += hdr.a_bss;
			setseg ((count + 077) >> 6);
			lseek (in, (off_t) sizeof (hdr), 0);
			count = hdr.a_text;
			if (sread (in, (char *) 0, count) != count)
				goto error;
			corep = 0;
			break;
# endif
		}
		corep += hdr.a_data;
# ifdef vax
		entry = (int (*)()) (hdr.a_entry & 0x3fffffff);
# endif
	}
# ifdef pdp11
	count = HBSS - (unsigned) corep;
	hdr.a_bss = hdr.a_bss < count ? hdr.a_bss : count;
# endif
	clrseg (corep, hdr.a_bss, KERNEL);
	close (in);
	return (0);
}

# define SCAN 0
# define FILL 1

getargv () {
	register unsigned count = 0;
	register char *quote, *ptr;
	int pass = SCAN;
	int shift = 0;
	extern char *strncpy ();
	extern char *strchr ();

	_prs ("$$ ");

	ptr = argbuf;
	fgets (argbuf, sizeof (argbuf) - 1, stdin);
	argbuf [sizeof (argbuf) - 1] = '\n';
	*strchr (argbuf, '\n') = '\0';

	for (;;) {
		while ((ptr-argbuf) < sizeof (argbuf) - 1) {
			while (*ptr) {
				if (*ptr != ' ' && *ptr != '\t')
					break;
				++ptr;
			}
			if (*ptr == '\0')
				break;
			if (pass == FILL)
				argv [count] = ptr;
			++count;
			while (*ptr) {
				if (*ptr == ' ' || *ptr == '\t')
					break;
				if (*ptr == '"' || *ptr == '\'') {
					quote = strchr (&ptr [1], *ptr);
					if (pass == FILL) {
						strncpy (ptr, &ptr [1], quote - ptr - 1);
						shift += 2;
					}
					ptr = quote;
				} else
					ptr [-shift] = *ptr;
				++ptr;
			}
			if (pass == FILL) {
				ptr [-shift] = '\0';
				shift = 0;
				++ptr;
			}
		}
		if (pass == FILL) {
			argc = count;
			return;
		}
		ptr = argbuf;
		count = 0;
		pass = FILL;
	}
}

# define UNIT	0
# define DISK	1

set () {
	register struct mtab *mp;
	register devno, stype;
	char unit, mname [NAMSIZ];

	if (argc == 1) {
		for (mp = &_mtab [0]; mp < &_mtab [NMOUNT]; mp++) {
			if (mp->mt_name == 0)
				continue;
			ppath (mp->mt_name, "\t");
			_prs (devname [mp->mt_dp->dt_devp - &_devsw [0]]);
			_prs (" ");
			unit = mp->mt_dp->dt_unit + '0';
			write (1, &unit, 1);
			_prs ("\n");
		}
		return;
	}

	if (argc != 4) {
		_prs ("Usage: set [unit {/|/usr} {0|1|...|7}]\n");
		_prs (SET2);
		return;
	}

	_cond (argv [2], mname);

	if (strcmp (argv [1], "unit") == 0) {
		stype = UNIT;

		unit = argv [3][0];

		if (unit < '0' || unit > '7' || argv [3][1] != '\0') {
			_prs ("set: bad unit number\n");
			return;
		} else
			unit -= '0';
	}
	if (strcmp (argv [1], "disk") == 0) {
		stype = DISK;

		for (devno = 0; devno < _devcnt; devno++)
			if (strcmp (argv [3], devname [devno]) == 0)
				break;
		if (devno == _devcnt) {
			_prs ("set: bad dev name\n");
			return;
		}
	}

	for (mp = &_mtab [0]; mp < &_mtab [NMOUNT]; mp++) {
		if (mp->mt_name == 0)
			continue;
		if (strcmp (mname, mp->mt_name) == 0) {
			switch (stype) {
			case UNIT:
				mp->mt_dp->dt_unit = (int) unit;
				return;
			
			case DISK:
				unit = (char) mp->mt_dp->dt_unit;
				mp->mt_dp = &_dtab [devno*2 + (&_mtab[NMOUNT-1] - mp)];
				mp->mt_dp->dt_unit = (int) unit;
				return;

			default:
				_prs ("set: bad type\n");
				return;
			}
		}
	}
	_prs ("set: bad fs name\n");
}

ppath (str, tc)
char *str, *tc; {

	if (str == NULL || *str == '\0')
		_prs ("/");
	else
		_prs (str);
	_prs (tc);
}

unsigned
sread (filep, addr, count)
register char *addr;
unsigned count; {
	register unsigned nleft;
	register cnt, nread;
	char buf[BUFSIZ];

	if (count == 0)
		return (0);
	nleft = count;
	do {
		cnt = (nleft > BLKSIZ) ? BLKSIZ : (int) nleft;
		if ((nread = read (filep, buf, cnt)) != cnt) {
			if (nread < 0)
				return (-1);
			cnt = 0;
		} else
			cnt = 1;
		movc3 ((unsigned) nread, buf, addr, USER);
		nleft -= (unsigned) nread;
		if (!cnt)
			return (count - nleft);
		addr += nread;
	} while (nleft != 0);
	return (count);
}
