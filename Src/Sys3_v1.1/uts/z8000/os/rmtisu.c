	register struct dfsdfs *msg;
	register struct dfswe *dwe;
	struct	dfswe	*gdfswe();
	int	spri;

	printf( "riput called %x\n", ip );
	debug();
	dwe = gdfswe();
	msg = (struct dfsdfs *)smalloc( sizeof( struct dfsdfs) );
	msg->ip_inode = ip->i_dfsi;
	dwe->we_type = DWEIP;
	dwe->we_parm1 = (int) msg;
	spri = spl0();
	adddwe( ip->i_dfsid, dwe );
	sleep( (caddr_t)dwe, DFSPRI );
	splx( spri );
	smfree( sizeof( struct dfsdfs), msg );
	rdfswe( dwe );
