char *
rjesys()
{
	static char u[8];
	register int t;
	u[0]=0;
	t=open("/usr/rje/sys",0);
	read(t,u,1);
	close(t);
	if (u[0]==0) u[0]='X';
	t=0377&u[0];
	if (t>=0140) t -= 040;
	if (t<'A' || t>'Z') t=0;
	u[0]=t; 
	u[1]=0;
	return (u);
};

