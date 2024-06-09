
fsfree(d,v)
 int v[];
 {int fsb[8];
  register int t;
  t=ustat(d,fsb);
  if (fsb[0]<0) fsb[0]=10000;
  if (fsb[1]<0) fsb[1]=10000;
  *v++=fsb[0];
  *v++=fsb[1];
  return (t);};
