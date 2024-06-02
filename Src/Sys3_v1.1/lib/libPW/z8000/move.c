move(a,b,n)
char *a,*b;
int n;
{
    register char *x,*y;
    register int m;

    for(m=n,x=a,y=b;m--;) *y++ = *x++;

}
