#include "sys/plexus.h"
#include "sys/param.h"
#include "sys/file.h"
#include "sys/dir.h"
#include "sys/maxuser.h"
#include "sys/user.h"
#include "sys/tty.h"

#include "icp/siocunix.h"		/* icp specific */

vpmopen(dev,flag)
dev_t dev;
int flag;
{
    register t;
    register i;
    register s;
    extern char legal_icp[];

    t=minor(dev);
    if(legal_icp[IDEV(t)]) {
	 u.u_error = 0;
         siopen(0, 0, flag, t, 1);
	 if(u.u_error)
	      return;
    }
    else
	u.u_error = ENXIO;
}
vpmclose(dev,flag)
dev_t dev;
int flag;
{
    register t;
    register s;

    t=minor(dev);
    u.u_error = 0;
    siclose(0, 0, t, 1);
}
vpmread(dev)
dev_t dev;
{
    register t;

    t=minor(dev);
    u.u_error = 0;
    siread(0, 0, t, 1);
}
vpmwrite(dev)
dev_t dev;
{
    register t;

    t=minor(dev);
    u.u_error = 0;
    siwrite(0, 0, t, 1);
}
vpmioctl(dev, cmd, arg, mode)
dev_t dev;
{
    register t;

    t=minor(dev);
    siioctl(0, 0, cmd, arg, mode, t, 1);
}
