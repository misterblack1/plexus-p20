/*
 *	indirect driver for controlling tty.
 */
#include "sys/param.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/conf.h"
#include "sys/tty.h"
#include "sys/proc.h"

syopen(dev, flag)
{

	if (sycheck())
	(*cdevsw[major(u.u_ttyd)].d_open)(minor(u.u_ttyd), flag);
}

syread(dev)
{

	if (sycheck())
	(*cdevsw[major(u.u_ttyd)].d_read)(minor(u.u_ttyd));
}

sywrite(dev)
{

	if (sycheck())
	(*cdevsw[major(u.u_ttyd)].d_write)(minor(u.u_ttyd));
}

syioctl(dev, cmd, arg, mode)
{

	if (sycheck())
	(*cdevsw[major(u.u_ttyd)].d_ioctl)(minor(u.u_ttyd), cmd, arg, mode);
}

sycheck()
{
	if (u.u_ttyp == NULL) {
		u.u_error = ENXIO;
		return(0);
	}
	if (u.u_ttyp->t_pgrp != u.u_procp->p_pgrp) {
		u.u_error = EIO;
		return(0);
	}
	return(1);
}
