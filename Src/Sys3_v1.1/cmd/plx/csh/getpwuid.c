/*
 * Modified version of getpwuid which doesn't use stdio.
 * This is done to keep it (and hence csh) small at a small
 * cost in speed.
 */
#include <pwd.h>
#include <whoami.h>

#ifdef UCB_PWHASH
#include <sys/pwdtbl.h>
#include <sys/stat.h>
#define pwf _pw_file

static char	UTBL[] = "/etc/uid_pw_map";	/* name of user id table */
extern int	pwf;				/* pointer to password file */
static int	uf = -1;			/* user id table */
static int	nusers;				/* number of users */
static off_t	uidloc();			/* finds uid from table */
struct stat	sbuf;				/* stat buffer */
#endif

struct passwd *
getpwuid(uid)
register uid;
{
	register struct passwd *p;
	struct passwd *getpwent();

#ifdef UCB_PWHASH
	off_t loc;
#endif

#ifdef UCB_PWHASH
	if (pwf == -1)
		setpwent();

	loc = uidloc (uid);
	if (loc >= 0)  {
		lseek (pwf,loc,0);
		p = getpwent();
		if (p->pw_uid == uid)
			return (p);
	}
#endif

						/* begin linear search */
	setpwent();
	while( (p = getpwent()) && p->pw_uid != uid );
	endpwent();
	return(p);
}

#ifdef UCB_PWHASH
static off_t
uidloc (uid)				/* finds uid from uid->loc table */
register unsigned	uid;
{
	off_t		high;			/* high pointer to file */
	off_t		low;			/* low pointer to file */
	off_t		test;			/* current pointer to file */
	struct pwloc	p;			/* table entry from file */

						/* get status of uid table file
						 * and open it */
	if (uf == -1)  {
		if (stat (UTBL,&sbuf) || (uf = open (UTBL, 0)) == -1)
			return ((off_t) -1);

						/* compute number of entries */
		nusers = sbuf.st_size/sizeof(struct pwloc);
	}

	low = 0;				/* initialize pointers */
	high = nusers;

compare:
	test = (high+low+1)/2;			/* find midpoint */
	if (test == high)			/* no valid midpoint */
		return ((off_t) -1);

						/* read table entry */
	lseek (uf, test*sizeof (struct pwloc), 0);
	if (read(uf, &p, sizeof (struct pwloc)) != sizeof (struct pwloc))
		return ((off_t) -1);
	if (p.pwl_uid == uid)			/* return if location found */
		return (p.pwl_loc);
	
	if (p.pwl_uid < uid)			/* adjust pointers */
		low = test;
	else
		high = test;
	goto compare;
}
#endif
