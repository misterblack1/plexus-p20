/*
**	strip -- remove symbol table from object files
*/

#include	<a.out.h>
#include	<signal.h>
#include	<stdio.h>
#include	<ar.h>

#define		BSIZE	512
#define		RELOC	struct ri
#define		RSIZE	sizeof(RELOC)

#ifdef	vax
RELOC	{
	long	addr;
	long	sn:24,
		flags:8;
} *reloc;
#else
RELOC	{
#ifdef STOCKIII
	short	d1:4,
		sn:11;
#else
	short	hibit:1,	/* fixes byte swap bug which caused */
		sn:11,		/* strip to screw up archives: the loader */
		d1:4;		/* reported a local symbol botch */
#endif
} *reloc;
#endif


struct	exec	head;
struct	ar_hdr	arhd;
struct	nlist	symt;

char	*tname;
char	*tname2;
char	*mktemp();
char	*malloc();

int	status;

FILE	*tf;

main(argc, argv)
char **argv;
{
	register i;

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	tname = mktemp("/tmp/sXXXXXX");
	close(creat(tname, 0600));
	tf = fopen(tname, "w+");
	if(tf == NULL) {
		fprintf(stderr, "strip: cannot create temp file\n");
		exit(2);
	}
	for(i=1; i<argc; i++) {
		strip(argv[i]);
		if(status > 1)
			break;
	}
	fclose(tf);
	unlink(tname);
	unlink(tname2);
	exit(status?2:0);
}

strip(name)
char *name;
{
	register	FILE	*f;
	int		mag, i;
	long		size;

	f = fopen(name, "r");
	if(f == NULL) {
		fprintf(stderr, "strip: cannot open %s\n", name);
		status = 1;
		goto out;
	}
	fread((char *)&mag, 1, sizeof(mag), f);	/* grab magic # */
	if(mag == ARMAG) {			/* it's an archive */
		FILE	*tf2;
		RELOC	*rend, *r;
		int	*extind;
		int	rsize;
		long	asize;

		asize = sizeof(mag);
		tname2 = mktemp("/tmp/sbXXXXXX");
		close(creat(tname2, 0600));
		tf2 = fopen(tname2, "w+");	/* update mode */
		if(tf2 == NULL) {
			fprintf(stderr, "strip: cannot create second temp\n");
			exit(2);
		}
		fwrite((char*)&mag, 1, sizeof(mag), tf);
						/* read archive headers */
		while(fread((char *)&arhd, 1, sizeof(arhd), f)) {
			char	nbuf[40];
			int	n;
			int	loc = 0;
			int	a = 0;

			asize += sizeof(arhd);
			fread((char *)&head, 1, sizeof(head), f);
			strcpy(nbuf, name);
			strcat(nbuf, ":");
			strcat(nbuf, arhd.ar_name);
			if(BADMAG(head)) {	/* not an a.out, continue */
				goto cont;
			}
						/* if a member is already */
						/* stripped, continue	  */
#ifdef	vax
			if((head.a_syms == 0) && (head.a_trsize == 0) &&
			   (head.a_drsize == 0)) {
#else
			if(head.a_syms == 0 && (head.a_flag&1) != 0) {
#endif
cont:
				fwrite((char *)&arhd, 1, sizeof(arhd), tf);
				fwrite((char *)&head, 1, sizeof(head), tf);
				size = arhd.ar_size - sizeof(head);
				if(arhd.ar_size&01) {
					asize++;
					size++;
				}
				copy(nbuf, f, tf, size);
				asize += arhd.ar_size;
				continue;
			}
			size = (long) head.a_text + head.a_data;
#ifdef	vax
			rsize = head.a_trsize + head.a_drsize;
#else
			rsize = (head.a_flag==0)?(head.a_text+head.a_data):0;
#endif
						/* copy T, D and read reloc   */
						/* leaving us at start of     */
						/* symbol table		      */
			copy(nbuf, f, tf2, size);
			n = head.a_syms / sizeof(symt);	/* number of syms */
			if((extind = (int *)malloc(n * sizeof(int))) == NULL ||
			   (reloc = (RELOC *)malloc(rsize)) == NULL) {
				fprintf(stderr, "strip: no memory!\n");
				status = 2;
				goto out;
			}
			if(fread((char *)reloc, 1, rsize, f) != rsize) {
				fprintf(stderr, "strip:read error on %s\n", nbuf);
				status = 2;
				goto out;
			}
			rend = &reloc[rsize / RSIZE];
			for(i = 0; i < n; i++) {
				fread((char *)&symt, 1, sizeof(symt), f);
				if((symt.n_type&N_EXT) == 0) { /* got a local */
					loc = 1;
					head.a_syms -= sizeof(symt);
					arhd.ar_size -= sizeof(symt);
					continue;
				}
				else
					extind[i] = a++;
				fwrite((char *)&symt, 1, sizeof(symt), tf2);
			}
			if(loc)
			for(r = reloc; r < rend; r++) {
#ifdef	vax
				if(r->flags&010)	/* is it external? */
#endif
				if(r->sn)
					r->sn = extind[r->sn];
			}
			fwrite((char *)&arhd, 1, sizeof(arhd), tf);
			fwrite((char *)&head, 1, sizeof(head), tf);
			asize += sizeof(head);
			fseek(tf2, 0L, 0);
			copy(nbuf, tf2, tf, size);
			fwrite((char *)reloc, 1, rsize, tf);
			copy(nbuf, tf2, tf, (long)head.a_syms);
			asize += head.a_syms + size + rsize;
			fseek(tf2, 0L, 0);
			free(reloc);
			free(extind);
		}
		size = asize;
	} else  {
		fseek(f, 0L, 0);
		fread((char *)&head, 1, sizeof(head), f);
		if(BADMAG(head)) {
			fprintf(stderr, "strip: %s not in a.out format\n",name);
			status = 1;
			goto out;
		}
#ifdef	vax
		if ((head.a_syms == 0) && (head.a_trsize == 0) && (head.a_drsize ==0)) {
#else
		if(head.a_syms == 0 && (head.a_flag&1) != 0) {
#endif
			fprintf(stderr, "strip: %s already stripped\n", name);
			status = 1;
			goto out;
		}
		size = (long)head.a_text + head.a_data;
		head.a_syms = 0;
#ifdef	vax
		head.a_trsize = head.a_drsize = 0;
#else
		head.a_flag |= 1;
#endif

		fwrite((char *)&head, 1, sizeof(head), tf);
		if(copy(name, f, tf, size)) {
			status = 1;
			goto out;
		}
		size += sizeof(head);
	}
	fclose(f);
	f = fopen(name, "w");
	if(f == NULL) {
		fprintf(stderr, "strip: cannot recreate %s\n", name);
		status = 2;
		goto out;
	}
	fseek(tf, 0L, 0);
	if(copy(name, tf, f, size))
		status = 2;
	fseek(tf, 0L, 0);

out:
	if(f)
		fclose(f);
}

copy(name, fr, to, size)
char *name;
FILE *fr, *to;
long size;
{
	register s, n;
	char buf[512];

	while(size != 0) {
		s = 512;
		if(size < 512)
			s = (int)size;
		n = fread(buf, 1, s, fr);
		if(n != s) {
			fprintf(stderr, "strip: unexpected eof in %s\n", name);
			return(1);
		}
		n = fwrite(buf, 1, s, to);
		if(n != s) {
			fprintf(stderr, "strip: write error in %s\n", name);
			return(1);
		}
		size -= s;
	}
	fflush(to);
	return(0);
}
