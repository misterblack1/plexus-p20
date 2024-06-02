#define nmag    0177545
#define omag    0177545

struct
{
        char    name[14];
        long    ndate;
        char    nuid;
        char    ngid;
        int     nmode;
        long    nsiz;
} nh;

struct
{
        char    oname[14];
        unsigned short date1;
        unsigned short date2;
        char    ouid;
        char    ogid;
        unsigned short  omode;
        unsigned short siz1;
        unsigned short siz2;
} oh;

char    *tmp;
int     f;
int     tf;
int     buf[256];

long    newmag = 0177545L;
main(argc, argv)
char *argv[];
{
        register i;

        tmp = mktemp("/tmp/arcXXXX");
        for(i=1; i<4; i++)
                signal(i, 1);
        for(i=1; i<argc; i++)
                conv(argv[i]);
        unlink(tmp);
}

conv(fil)
char *fil;
{
        register i, n;

        f = open(fil, 2);
        if(f < 0) {
                printf("cannot open %s\n", fil);
                return;
        }
        close(creat(tmp, 0600));
        tf = open(tmp, 2);
        if(tf < 0) {
                printf("cannot open temp\n");
                close(f);
                return;
        }
        buf[0] = 0;
        read(f, buf, 2);
        if(buf[0] != omag) {
                printf("not archive format\n");
                close(tf);
                close(f);
                return;
        }
	read(f, buf, 2);
	if(buf[0] == 0) {

		printf("file already converted\n");
		close(tf);
		close(f);
		return;
	}
	lseek(f, -2L, 1);
        write(tf, &newmag, 4);
loop:
        i = read(f, &oh, sizeof(oh));
        if(i != sizeof oh)
                goto out;
        for(i=0; i<14; i++)
                nh.name[i] = oh.oname[i];
        nh.nsiz = (long)oh.siz1<<16 | oh.siz2;
        nh.nuid = oh.ouid;
        nh.ngid = oh.ogid;
        nh.nmode = oh.omode;
        nh.ndate = (long) oh.date1<<16 | oh.date2;
        n = ((nh.nsiz+1) >> 1) ;
        write(tf, &nh, sizeof(nh));
        while(n > 0) {
                i = 256;
                if(n < i)
                        i = n;
                read(f, buf, i+i);
                write(tf, buf, i+i);
                n -= i;
        }
        goto loop;
out:
        lseek(f, 0L, 0);
        lseek(tf, 0L, 0);
        while((i=read(tf, buf, 512)) > 0)
                write(f, buf, i);
        close(f);
        close(tf);
}
