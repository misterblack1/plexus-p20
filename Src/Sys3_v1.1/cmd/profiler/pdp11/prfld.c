#include <sys/plexus.h>
struct	symbol	{
	char	s_name[8];
	unsigned	s_type;
	unsigned	s_value;
} ;

struct	aout	{
	unsigned	a_mag;
	unsigned	a_text;
	unsigned	a_data;
	unsigned	a_bss;
	unsigned	a_sym;
	unsigned	a_ent;
	unsigned	a_un;
	unsigned	a_rel;
} ;

char	*namelist = UNIXNAME;
struct	symbol	symbol;
struct	aout	aout;
unsigned	symcnt;
long	symloc;
int	taddr[1024];

main(argc, argv)
	int	argc;
	char	**argv;
{
	register  int  *ip, nm, prf;
	int	compar();

	if(argc == 2)
		namelist = argv[1];
	else if(argc != 1)
		error("usage: prfld [%s]\n",UNIXNAME);
	if((nm = open(namelist, 0)) < 0)
		error("cannot open namelist file\n");
	if((prf = open("/dev/prf", 1)) < 0)
		error("cannot open /dev/prf\n");
	if(read(nm, &aout, sizeof aout) != sizeof aout)
		error("cannot read namelist file\n");
	if(aout.a_sym == 0)
		error("no namelist\n");

	symcnt = aout.a_sym / sizeof symbol;
	if(aout.a_rel)
		symloc = 020 + (long)aout.a_text + (long)aout.a_data;
	else
		symloc = 020 + 2 * ((long)aout.a_text + (long)aout.a_data);
	lseek(nm, symloc, 0);
	ip = taddr;
	*ip++ = 0;
	while(symcnt--) {
		if(ip == &taddr[1024])
			error("too many text symbols\n");
		if(read(nm, &symbol, sizeof symbol) != sizeof symbol)
			error("cannot read namelist\n");
		if(symbol.s_type == 042)
			*ip++ = symbol.s_value;
	}
	qsort(taddr, ip - taddr, sizeof(int), compar);
	if(write(prf, taddr, (unsigned)ip - (unsigned)taddr) !=
	    (unsigned)ip - (unsigned)taddr)
		error("cannot load profile addresses\n");
}

compar(x, y)
	register  unsigned  *x, *y;
{
	if(*x > *y)
		return(1);
	else if(*x == *y)
		return(0);
	return(-1);
}

error(s)
	register  char  *s;
{
	write(2, "prfld: ", 7);
	write(2, s, strlen(s));
	exit(1);
}
