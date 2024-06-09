#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/file.h"
#include "sys/dir.h"
#include "sys/maxuser.h"
#include "sys/user.h"
#include "icp/siocunix.h"		/* icp specific */

char legal_line[NUMICP*8];		/* vector of active r/w lines */

spopen(dev,flag)
dev_t dev;
int flag;
{
    register t;
    register i;
    register s;
    extern char legal_icp[];
    extern int numuser;
    t=minor(dev);
    if(legal_icp[t/NUMSIO]) {
	s=spl4();
	if(legal_line[t]) {
	    splx(s);
#ifdef VPMSYS
	    siopen(t/NUMSIO, t%NUMSIO, flag, t, 0);
#else
	    siopen(t/NUMSIO, t%NUMSIO, flag);
#endif
	}
	else {
	    if((flag&(FREAD|FWRITE)) == (FREAD|FWRITE)) {
		if(numuser >= MAXUSER) {
	    	    splx(s);
		    u.u_error = EINVAL;
		}
		else {
	    	    splx(s);
		    u.u_error = 0;
#ifdef VPMSYS
    		    siopen(t/NUMSIO, t%NUMSIO, flag, t, 0);
#else
		    siopen(t/NUMSIO, t%NUMSIO, flag);
#endif
		    if(u.u_error)
			return;
		    s=spl4();
		    legal_line[t]++;
		    numuser++;
	    	    splx(s);
		}
	    }
	    else {
	    	splx(s);
#ifdef VPMSYS
    	    	siopen(t/NUMSIO, t%NUMSIO, flag, t, 0);
#else
		siopen(t/NUMSIO, t%NUMSIO, flag);
#endif
	    }
	}
    }
    else
	u.u_error = ENXIO;
}
spclose(dev,flag)
dev_t dev;
int flag;
{
    register t;
    register s;
    extern int numuser;
    t=minor(dev);
    u.u_error = 0;
#ifdef VPMSYS
    siclose(t/NUMSIO, t%NUMSIO, t, 0);
#else
    siclose(t/NUMSIO, t%NUMSIO);
#endif
    if(u.u_error)
	return;
    /*
     * On last close of tty line, clear flag "line open for Read/Write" for
     * this line. Also decrement number of lines thus opened.
     */
    s=spl6();
    if(legal_line[t])
	numuser--;
    legal_line[t] = 0;
    splx(s);
}
spread(dev)
dev_t dev;
{
    register t;
    t=minor(dev);
#ifdef VPMSYS
    siread(t/NUMSIO, t%NUMSIO, t, 0);
#else
    siread(t/NUMSIO, t%NUMSIO);
#endif
}
spwrite(dev)
dev_t dev;
{
    register t;
    t=minor(dev);
#ifdef VPMSYS
    siwrite(t/NUMSIO, t%NUMSIO, t, 0);
#else
    siwrite(t/NUMSIO, t%NUMSIO);
#endif
}
spioctl(dev, cmd, arg, mode)
dev_t dev;
{
    register t;
    t=minor(dev);
#ifdef VPMSYS
    siioctl(t/NUMSIO, t%NUMSIO, cmd, arg, mode, t, 0);
#else
    siioctl(t/NUMSIO, t%NUMSIO, cmd, arg, mode);
#endif
}
