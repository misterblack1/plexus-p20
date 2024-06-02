/*
 * Declaration of block device switch. Each entry (row) is
 * the only link between the main unix code and the driver.
 * The initialization of the device switches is in the file conf.c.
 */
extern struct bdevsw
{
	int	(*d_open)();
	int	(*d_close)();
	int	(*d_strategy)();
	struct iobuf *d_tab;
} bdevsw[];

/*
 * Character device switch.
 */
extern struct cdevsw
{
	int	(*d_open)();
	int	(*d_close)();
	int	(*d_read)();
	int	(*d_write)();
	int	(*d_ioctl)();
} cdevsw[];

#ifndef UNIX_ICP		/* don't include with unix<->icp software */
int	bdevcnt;
int	cdevcnt;
#endif

/*
 * Line discipline switch.
 */
extern struct linesw
{
	int	(*l_open)();
	int	(*l_close)();
	int	(*l_read)();
	int	(*l_write)();
	int	(*l_ioctl)();
	int	(*l_input)();
	int	(*l_output)();
	int	(*l_mdmint)();
} linesw[];

#ifdef ICP /* set linecnt for ICP to same value as used in cf. */
#define linecnt 1
#else
#ifndef UNIX_ICP	/* don't want linecnt in unix<->icp code */
int	linecnt;
#endif
#endif

/*
 * device controller initialization switch
 */
extern struct initsw
{
	int	(*d_init)();
} initsw[];

