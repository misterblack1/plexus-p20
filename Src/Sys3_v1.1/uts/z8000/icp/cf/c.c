/*
 * static data areas used by the sioc.
 */

#include "sys/conf.h"

int	nulldev();

int	nodev();

int	sioopen(), sioclose(), sioread(), siowrite(), sioioctl();

struct	cdevsw	cdevsw[] =
{
	sioopen, sioclose, sioread, siowrite, sioioctl, 0, /* 0 */
	0
};

int pdmac[12] = {	/* logical dma unit ctrl to physical port map */
	0x4198,
	0x4198,
	0x4198,
	0x4198,
	0x4298,
	0x4298,
	0x4298,
	0x4298,
	0x4098,
	0x4098,
	0x4098,
	0x4098,
};

int pdmaa[12] = {	/* logical dma unit adx to physical port map */
	0x4190,
	0x4192,
	0x4194,
	0x4196,
	0x4290,
	0x4292,
	0x4294,
	0x4296,
	0x4090,
	0x4092,
	0x4094,
	0x4096,
};

int pdmax[12] = {	/* logical dma unit cnt to physical port map */
	0x4191,
	0x4193,
	0x4195,
	0x4197,
	0x4291,
	0x4293,
	0x4295,
	0x4297,
	0x4091,
	0x4093,
	0x4095,
	0x4097,
};

int ppioc[2] = {	/* logical pio unit ctrl to physical port map */
	0x40e2,
	0x40e3,
};

int ppiod[2] = {	/* logical pio unit data to physical port map */
	0x40e0,
	0x40e1,
};

int pctc[16] = {	/* logical ctc unit to physical port map */
	0x4000,
	0x4001,
	0x4002,
	0x4010,
	0x4011,
	0x4012,
	0x4030,
	0x4031,
	0x4020,
	0x4021,
	0x4022,
	0x4023,
	0x4003,
	0x4013,
	0x4032,
	0x4033,
};

int psiod[8] = {	/* logical sio unit data to physical port map */
	0x4040,
	0x4041,
	0x4050,
	0x4051,
	0x4060,
	0x4061,
	0x4070,
	0x4071,
};

int psioc[8] = {	/* logical sio unit cmd to physical port map */
	0x4042,
	0x4043,
	0x4052,
	0x4053,
	0x4062,
	0x4063,
	0x4072,
	0x4073,
};

int	cmdsav;		/* saves the value in the hardware cmd latch */

int	defspeed;	/* default speed index */

long int canch;		/* sysinfo information. not yet implemented */
long int outch;
long int rawch;
long int rcvint;
