#include "sys/param.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "icp/siocunix.h"		/* icp specific */

	/*-------------------------------------*\
	| parallel port unit # 0 through NUMICP |
	\*-------------------------------------*/

ppopen(dev,flag)
dev_t dev;
int flag;
{
    register t;
    extern char legal_icp[];
    t=minor(dev);
    if(legal_icp[t])
	siopen(t,PIOUNIT,flag);
    else
	u.u_error = ENXIO;
	
}
ppclose(dev,flag)
dev_t dev;
int flag;
{
    register t;
    t=minor(dev);
    siclose(t,PIOUNIT);
}
	/* read from parallel port doesn't make sense */
ppread(dev)
dev_t dev;
{
    u.u_error = ENXIO;
}
ppwrite(dev)
dev_t dev;
{
    register t;
    t=minor(dev);
    siwrite(t,PIOUNIT);
}
	/* ioctl of parallel port doesn't make sense */
ppioctl(dev, cmd, arg, mode)
dev_t dev;
{
#ifdef VPR
    register t;
    t=minor(dev);
    siioctl(t/NUMSIO, PIOUNIT, cmd, arg, mode);
#else
    u.u_error = ENXIO;
#endif
}
