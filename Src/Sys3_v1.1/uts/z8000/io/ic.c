#include "sys/param.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "icp/siocunix.h"	/* icp specific */

	/*--------------------------------*\
	| download unit # 0 through NUMICP |
	\*--------------------------------*/

icopen(dev,flag)
dev_t dev;
int flag;
{
    register t;
    extern char legal_icp[];
    t=minor(dev);
    if(legal_icp[t])
#ifdef VPMSYS
	siopen(t,DNLDUNIT,flag,0,0);
#else
	siopen(t,DNLDUNIT,flag);
#endif
    else
	u.u_error = ENXIO;
}
icclose(dev,flag)
dev_t dev;
int flag;
{
    register t;
    t=minor(dev);
#ifdef VPMSYS
    siclose(t,DNLDUNIT,0,0);
#else
    siclose(t,DNLDUNIT);
#endif
}
icread(dev)
dev_t dev;
{
    register t;
    t=minor(dev);
#ifdef VPMSYS
    siread(t,DNLDUNIT,0,0);
#else
    siread(t,DNLDUNIT);
#endif
}
icwrite(dev)
dev_t dev;
{
    register t;
    t=minor(dev);
#ifdef VPMSYS
    siwrite(t,DNLDUNIT,0,0);
#else
    siwrite(t,DNLDUNIT);
#endif
}
	/* ioctl of down load port makes no sense */
icioctl(dev, cmd, arg, mode)
dev_t dev;
{
    u.u_error = ENXIO;
}
