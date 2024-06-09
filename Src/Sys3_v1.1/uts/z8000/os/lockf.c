#include "sys/param.h"
#include "sys/inode.h
#include "sys/file.h"
#include "sys/proc.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/systm.h"

#define MAXSIZE (long)(1L<<30)  /* number larger than any request */

/*
 * Handle syscall requests.
 */

locking()
{
	struct file *fp;
	struct inode *ip;
	register struct locklist *cl;
	register struct locklist *nl;
	off_t LB;
	off_t UB;
	register struct a {
		int fdes;
		int flag;
		off_t size;
	} *uap = (struct a *) u.u_ap;

	/*
	 * Check for valid open file
	 */

	if ((fp = getf(uap->fdes)) == NULL) {
		return;
	}
	ip = fp->f_inode;
	if ((ip->i_flag & IFMT) == IFDIR) {
		u.u_error = EACCES;
		return;
	}

	/*
	 * Validate range and handle special case of length 0
	 */

	LB = fp->f_un.f_off;
	if (uap->size) {
		UB = LB + uap->size;
		if (UB <= 0) {
			UB = MAXSIZE;
		}
	} else {
		UB = MAXSIZE;
	}

	/*
	 * Test for unlock request
	 */

	if (uap->flag == 0) {

		/*
		 * Start at list head and scan for locks in range for
		 * this process.
		 */
		cl = (struct locklist*) &ip->i_locklist;
		while (nl = cl->ll_link) {

			/*
			 * if not for this process skip to next lock
			 */

			if (nl->ll_proc != u.u_procp) {
				cl = nl;
				continue;
			}

			/*
			 * check for locks in proper range
			 */

			if (UB <= nl->ll_start) {
				break;
			}
			if (nl->ll_end <= LB) {
				cl = nl;
				continue;
			}

			/*
			 * for locks fully contained witn in
			 * requested range, just delete the item.
			 */

			if (LB <= nl->ll_start && nl->ll_end <= UB) {
				cl->ll_link = nl->ll_link;
				lockfree(nl);
				continue;
			}

			/*
			 * if someone is sleeping on this lock
			 * do a wakeup since the region being
			 * slept on may be freed.
			 */

			if (nl->ll_flags & IWANT) {
				nl->ll_flags &= ~IWANT;
				wakeup(nl);
			}

			/*
			 * Middle section is being removed.
			 * Add a new lock for last section and
			 * modify existing lock for first section.
			 * If no locks, return an error.
			 */

			if (nl->ll_start < LB && UB < nl->ll_end) {
				if (lockadd(nl, UB, nl->ll_end)) {
					return;
				}
				nl->ll_end = LB;
				break;
			}

			/*
			 * First section is being deleted, just
			 * move starting point up.
			 */

			if (LB <= nl->ll_start && UB < nl->ll_end) {
				nl->ll_start = UB;
				break;
			}

			/*
			 * Deleting last part of section, continue looking
			 * for locks covered by upper limit of unlock range.
			 * Move ending point down.
			 */

			nl->ll_end = LB;
			cl = nl;
		}

		/* 
		 * end of scan for unlock request.
		 */

		return;
	}

	/*
	 * Request must be a lock of some kind. Check to see
	 * if the region is lockable by this process.
	 */

	if (locked(uap->flag, ip, LB, UB)) {
		return;
	}
	cl = (struct locklist*) &ip->i_locklist;

	/*
	 * No existing locks, just add new lock.
	 */

	if ((nl = cl->ll_link) == NULL) {
		lockadd(cl, LB, UB);
		return;
	}

	/*
	 * Lock is before any other, insert at head of list.
	 */

	if (UB < nl->ll_start) {
		lockadd(cl, LB, UB);
		return;
	}

	/*
	 * Ending range of lock is same as start of lock by another
	 * process, insert at head of list.
	 */

	 if (UB <= nl->ll_start && u.u_procp != nl->ll_proc) {
		lockadd(cl, LB, UB);
		return;
	}

	/*
	 * Request range overlaps with beginning of fisrt request.
	 * Modify starting point in lock to include requested region.
	 */

	if (UB >= nl->ll_start && LB < nl->ll_start) {
		nl->ll_start = LB;
	}

	/*
	 * Scan thru the rest of the lock list
	 */

	cl = nl;
	do {
		/*
		 * actions for requests at the end of list
		 */

		if ((nl = cl->ll_link) == NULL) {
			
			/*
			 * request overlaps tail of last entry,
			 * extend end point.
			 */

			if (LB <= cl->ll_end && u.u_procp == cl->ll_proc) {
				if (UB > cl->ll_end) {
					cl->ll_end = UB;
				}
				return;
			}

			/*
			 * Otherwise add new entry.
			 */

			lockadd(cl, LB, UB);
			return;
		}

		/*
		 * If more locks in range do next, otherwise stop.
		 */

		if (nl->ll_start < LB) {
			cl = nl;
		} else {
			break;
		}
	} while (1);

	/*
	 * If upper bound is fully resolved we are done, otherwise
	 * fix end of last entry or ad new entry.
	 */

	if (UB <= cl->ll_end) {
		return;
	}
	if (LB <= cl->ll_end && u.u_procp == cl->ll_proc) {
		cl->ll_end = UB;
	} else {
		if (lockadd(cl, LB, UB)) {
			return;
		}
		cl = cl->ll_link;
	}

	/*
	 * End point set above may overlap later entries. If so
	 * delete or modify them to proform the compatcion.
	 */

	while ((nl = cl->ll_link) != NULL) {
		
		/*
		 * If a lock by another process is found then done
		 * since validation was done above.
		 */

		if (u.u_procp != nl->ll_proc) {
			return;
		}

		/*
		 * If the new endpoint no longer overlaps then done.
		 */

		if (cl->ll_end < nl->ll_start) {
			return;
		}

		/*
		 * If the new range overlaps the first part of the 
		 * next lock, take its end point and delete the
		 * next lock. Should be done.
		 */

		if (cl->ll_end <= nl->ll_end) {
			cl->ll_end = nl->ll_end;
			cl->ll_link = nl->ll_link;
			lockfree(nl);
			return;
		}

		/*
		 * The next lock is fully included in the new range
		 * so it may be deleted.
		 */

		cl->ll_link = nl->ll_link;
		lockfree(nl);
	}
}


/*
 * Scan locks and check for a locked condition.
 */

locked(flag, ip, LB, UB)
int flag;
register struct inode *ip;
off_t LB;
off_t UB;
{
	register struct locklist *nl;

	nl = ip->i_locklist;
	while (nl != NULL && nl->ll_start < UB) {
		
		/*
		 * Skip locks for this process and those out of range
		 */

		if (nl->ll_proc == u.u_procp || nl->ll_end <= LB) {
			nl = nl->ll_link;
			if (nl == NULL) {
				return(NULL);
			}
			continue;
		}

		/*
		 * Must have found lock by another process. If
		 * request is to test only, then exit with error
		 * code.
		 */

		if (flag > 1) {
			u.u_error = EACCES;
			return(1);
		}

		/*
		 * Will need to sleep on lock, check for deadlock first
		 * and abort on error.
		 */

		if (deadlock(nl) != NULL) {
			return(1);
		}

		/*
		 * Post the want flag then sleep.
		 */

		nl->ll_flags |= IWANT;
		sleep((caddr_t) nl, PSLEP);

		/*
		 * Rescan from beginning to catch any new area locked
		 * or a partial delete.
		 */

		nl = ip->i_locklist;

		/*
		 * abort if error
		 */

		if (u.u_error) {
			return(1);
		}
	}
	return(NULL);
}

/*
 * Find deadlocks on file locks.
 */

deadlock(lp)
register struct locklist *lp;
{
	register struct locklist *nl;

	/*
	 * Scan while the process owning the lock is sleeping
	 */

	while (lp->ll_proc->p_stat == SSLEEP) {
		
		/*
		 * If the object the process is sleeping on is 
		 * not in the lock table then everything is ok.
		 */

		nl = (struct locklist *) lp->ll_proc->p_wchan;
		if (nl < &locklist[0] || nl >= &locklist[NFLOCKS]) {
			break;
		}

		/*
		 * The object was a locklist entry. If the owner of
		 * that entry is this process then a deadlock would
		 * occur. In this case return an error.
		 */

		if (nl->ll_proc == u.u_procp) {
			u.u_error = EDEADLOCK;
			return((int) nl);
		}

		/*
		 * The object was a locklist entry owned by some
		 * other process. Continue the scan with that 
		 * process.
		 */

		lp = nl;
	}
	return(NULL);
}

/*
 * Release all locks for this process. Usually called when the file is
 * closed.
 */

unlock(ip)
register struct inode *ip;
{
	register struct locklist *nl;
	register struct locklist *cl;

	if (cl = (struct locklist *) &ip->i_locklist) {
		while ((nl = cl->ll_link) != NULL) {
			if (nl->ll_proc == u.u_procp) {
				cl->ll_link = nl->ll_link;
				lockfree(nl);
			} else {
				cl = nl;
			}
		}
	}
}


/*
 * Allocate a lock entry
 */

struct locklist *lockalloc()
{
	register struct locklist *fl;
	register struct locklist *nl;

	fl = &locklist[0];

	/*
	 * If first entry has never been used link the locklist
	 * table into the freelist.
	 */

	if (fl->ll_proc == NULL) {
		fl->ll_proc = &proc[0];
		for (nl = &locklist[1]; nl < &locklist[NFLOCKS]; nl++) {
			lockfree(nl);
		}
	}

	/*
	 * If all the locks are used return an error.
	 */

	if ((nl = fl->ll_link) == NULL) {
		u.u_error = EDEADLOCK;
		return(NULL);
	}

	/*
	 * Return the next lock on the list.
	 */

	fl->ll_link = nl->ll_link;
	nl->ll_link = NULL;
	return(nl);
}


/*
 * Return a lock entry to the free list.
 */

lockfree(lp)
register struct locklist *lp;
{
	register struct locklist *fl;

	fl = &locklist[0];

	/*
	 * if any process is sleeping on this lock, wake it up.
	 */

	if (lp->ll_flags & IWANT) {
		lp->ll_flags &= ~IWANT;
		wakeup(lp);
	}

	/*
	 * Put the lock on the free list.
	 */
	
	lp->ll_link = fl->ll_link;
	fl->ll_link = lp;
}


/*
 * Add a lock entry to a lock list.
 */

lockadd(cl, LB, UB)
register struct locklist *cl;
off_t LB;
off_t UB;
{
	register struct locklist *nl;

	/*
	 * Get a lock, return if none available.
	 */

	if ((nl = lockalloc()) == NULL) {
		return(1);
	}

	/*
	 * link the new entry into the list at the current spot.
	 * fill in the data from the arguments.
	 */

	nl->ll_link = cl->ll_link;
	cl->ll_link = nl;
	nl->ll_proc = u.u_procp;
	nl->ll_start = LB;
	nl->ll_end = UB;
	return(0);
}
