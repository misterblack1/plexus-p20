#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
struct sa {
	struct sysinfo si;
	long d0;
	long d1;
	long d2;
	long ts;
	} x, xb, xtemp;
char	*optarg;
int	optind;
#define	PTS	45
float	xh, tix, s[PTS][26], smax[26];
float	hs, he, ymin, ymax;
char	term[10], date[10], hms_s[10], hms_e[10], linea[80], lineb[80];
int	ip, iplot, item[3];
float	yrange();
char	*strcpy(), *strncat(), *strcat();

#define	max(I, J)	(I>=J ? I : J)

main(argc, argv)
char	**argv;
int	argc;
{
int	i, bflag = 0;
char	options[20], binfile[20];
char	c;
struct	tm	*localtime(), *nowtm, args, arge, *xtm;
long	now, time(), totix;
int	fbin;

/*	Set defaults		*/

strcpy(hms_s, "08:00");
strcpy(hms_e, "18:00");
strcpy (term, "");
strcpy (options, "");
now = time ((long *) 0);
nowtm = localtime (&now);
sprintf(binfile, "/usr/adm/sa/sa%.2d", nowtm->tm_mday);

/*	Examine options; select -s, -e, -T; pack others into options	*/
/*	Get name of source file		*/

while ((i=getopt(argc,argv,"s:e:T:uirwcohdpaf")) != EOF)
    {
	switch (c=i) {
	case 's':
		sscanf(optarg, "%s", hms_s);
		break;
	case 'e':
		sscanf(optarg, "%s", hms_e);
		break;
	case 'T':   /*   terminal for plotting   */
		sprintf(term, "-T%s", optarg);
		break;
	case '?':
		printf ("Usage: sag [-s hh:mm] [-e hh:mm] [-T term] [-uirwcohdpaf] [file]\n");
	default:
		strncat (options,  &c, 1);
		break;
	}
    }
if (optind < argc)	strcpy (binfile, argv[optind]);
if ((i=sscanf(hms_s,"%d:%d",&args.tm_hour,&args.tm_min)) < 1)
	{
	printf("-s option arg: hh[:mm]\n");
	exit(0);
	}
if (i < 2)	strcat (hms_s, ":00");
if ((i=sscanf(hms_e,"%d:%d",&arge.tm_hour,&arge.tm_min)) < 1)
	{
	printf("-e option arg: hh[:mm]\n");
	exit(0);
	}
if (i < 2)	strcat (hms_e, ":00");
hs = (float)args.tm_hour + (float)args.tm_min/60;
he = (float)arge.tm_hour + (float)arge.tm_min/60;
if (he <= hs)
	{
	printf ("Bad range for hs - he: %f - %f\n", hs, he);
	exit(0);
	}

/*	Read data from binfile.
	When data time is within range, calculate
	1st differences and store plottable data items
	in s[PTS][26] array.
	If reboot record is encountered, set all
	s values negative and restart 1st differences.	*/

if ((fbin = open (binfile, 0)) < 0)
	{
	printf ("Can't read %s\n", binfile);
	exit(0);
	}

for (ip=0; ip<PTS;   )
	{
	if (read (fbin, &x, sizeof x) <= 0)	break;
	xtm = localtime (&x.ts);
	xh = (float)xtm->tm_hour + (float)xtm->tm_min/60 + (float)xtm->tm_sec/3600;
	if (((int)xh >= (int)hs) && ((int)xh <= (int)he))
		{
		if (bflag++)
			{
			totix =   x.si.cpu[CPU_IDLE]
				+ x.si.cpu[CPU_USER]
				+ x.si.cpu[CPU_KERNAL];
			if (totix <= 0)
				{
				bflag = 1;
				x.si.cpu[CPU_IDLE] = 0;
				x.si.cpu[CPU_USER] = 0;
				x.si.cpu[CPU_KERNAL] = 0;
				s[ip][0] = xh;
				for(i=1; i< 26; i++)
					s[ip][i] = -10000.;
				}
			else	{
   s[ip][0] = xh;
   tix = (float)((x.si.cpu[CPU_IDLE]	- xb.si.cpu[CPU_IDLE])
   	+ (x.si.cpu[CPU_USER]	- xb.si.cpu[CPU_USER])
   	+ (x.si.cpu[CPU_KERNAL]	- xb.si.cpu[CPU_KERNAL]));
   s[ip][1] = (float)(x.si.cpu[CPU_USER]	- xb.si.cpu[CPU_USER])	* 100/tix;
   s[ip][2] = (float)((x.si.cpu[CPU_USER]	- xb.si.cpu[CPU_USER])
   	+ (x.si.cpu[CPU_KERNAL]		- xb.si.cpu[CPU_KERNAL])) * 100./tix;
   s[ip][3] = (float)(x.si.wait[W_IO]	- xb.si.wait[W_IO])	* 100/tix;
   s[ip][4] = (float)(x.si.wait[W_SWAP]	- xb.si.wait[W_SWAP])	* 100/tix;
   s[ip][5] = (float)(x.si.wait[W_PIO]	- xb.si.wait[W_PIO])	* 100/tix;
   s[ip][6] = (float)(x.si.bread	- xb.si.bread)	*3600./tix;
   s[ip][7] = (float)(x.si.bwrite	- xb.si.bwrite)	*3600./tix;
   s[ip][8] = (float)(x.si.lread	- xb.si.lread)	*3600./tix;
   s[ip][9] = (float)(x.si.lwrite	- xb.si.lwrite)	*3600./tix;
   s[ip][10] = (float)(x.si.swapin	- xb.si.swapin)	*3600./tix;
   s[ip][11] = (float)(x.si.swapout	- xb.si.swapout)	*3600./tix;
   s[ip][12] = (float)(x.si.pswitch	- xb.si.pswitch)	*60./tix;
   s[ip][13] = (float)(x.si.preempt	- xb.si.preempt)	*60./tix;
   s[ip][14] = (float)(x.si.syscall	- xb.si.syscall)	*60./tix;
   s[ip][15] = (float)(x.si.iget	- xb.si.iget)	*60./tix;
   s[ip][16] = (float)(x.si.namei	- xb.si.namei)	*60./tix;
   s[ip][17] = (float)(x.si.dirblk	- xb.si.dirblk)	*60./tix;
   s[ip][18] = (float)(x.si.readch	- xb.si.readch)	*60./tix;
   s[ip][19] = (float)(x.si.writech	- xb.si.writech)	*60./tix;
   s[ip][20] = (float)(x.d0	- xb.d0)	*3600./tix;
   s[ip][21] = (float)(x.d1	- xb.d1)	*3600./tix;
   s[ip][22] = (float)(x.d2	- xb.d2)	*3600./tix;
   s[ip][23] = s[ip][6] + s[ip][7];
   s[ip][24] = 100* (1.- s[ip][6]/s[ip][8]);
   s[ip][25] = 100* (1.- s[ip][7]/s[ip][9]);
				}
			for (i=0; i<26; i++)
				smax[i] = max(smax[i], s[ip][i]);
			ip++;
			}
		xtemp = xb;
		xb = x;
		x = xtemp;
		}
	}
close (fbin);

sprintf(date, "%.2d/%.2d/%.2d", 
	xtm->tm_mon + 1,
	xtm->tm_mday,
	xtm->tm_year);

/*	For each option requested, set up labels
	and declare s[ip][item]'s  to be plotted			*/

i = 0;
do	{
	c = options[i];
	if (setup(c))	plotit();

	if (c == NULL)		break;
	}
while ((c=options[++i]) != NULL);

}

setup(c)
char	c;

{
float	temp;

ymin = 0;
strcpy(lineb, "");
	switch (c){
	case '\0':
	case 'u':
		strcpy(linea, "CPU Utilization vs Hour");
		strcpy(lineb, "bottom space: user   middle: sys   top: idle");
		iplot = 2;
		item[0] = 1;
		item[1] = 2;
		ymax = 100;
		break;

	case 'i':
		strcpy (linea, "% of CPU Time Idle and Waiting for I/O  vs Hour");
		strcpy (lineb, "+: wait bio   *: wait swap   =: wait pio");
		iplot = 3;
		item[0] = 3;
		item[1] = 4;
		item[2] = 5;
		temp = max (smax[3], smax[4]);
		ymax = yrange (max (temp, smax[5]));
		break;
	case 'r':
		strcpy (linea, "Read Activity vs Hour");
		strcpy (lineb, "+: block reads/min;   *: logical reads/min");
		iplot = 2;
		item[0] = 6;
		item[1] = 8;
		ymax = yrange (max (smax[6], smax[8]));
		break;

	case 'w':
		strcpy (linea, "Write Activity vs Hour");
		strcpy (lineb, "+: block writes/min   *: logical writes/min");
		iplot = 2;
		item[0] = 7;
		item[1] = 9;
		ymax = yrange (max (smax[7], smax[9]));
		break;

	case 'c':
		strcpy (linea, "Buffer Cache Hit Ratio %  vs Hour");
		strcpy (lineb, "+: cached reads   *: cached writes");
		iplot = 2;
		item[0] = 24;
		item[1] = 25;
		ymax = yrange (max (smax[24], smax[25]));
		break;

	case 'o':
		strcpy (linea, "Blocks Transferred Between System Buffers & Devices");
		strcpy (lineb, "+: writes/min   *: reads/min   =: writes+reads/min");
		iplot = 3;
		item[0] = 7;
		item[1] = 6;
		item[2] = 23;
		temp = max (smax[7], smax[6]);
		ymax = yrange (max (temp, smax[23]));
		break;

	case 'h':
		strcpy (linea, "Bytes(characters) Transferred  vs Hour");
		strcpy (lineb, "+: bytes written/sec   *: bytes read/sec");
		iplot = 2;
		item[0] = 19;
		item[1] = 18;
		ymax = yrange (max (smax[19], smax[18]));
		break;

	case 'd':
		strcpy (linea, "Disk Device Reads+Writes/min  vs Hour");
		strcpy (lineb, "+: D0   *: D1   =: D2");
		iplot = 3;
		item[0] = 20;
		item[1] = 21;
		item[2] = 22;
		temp = max (smax[20], smax[21]);
		ymax = yrange (max (temp, smax[22]));
		break;

	case 'p':
		strcpy (linea, "Process Switching & System Calls  vs Hour");
		strcpy (lineb, "+: preempt/sec   *: pswitch/sec   =: syscall/sec");
		iplot = 3;
		item[0] = 13;
		item[1] = 12;
		item[2] = 14;
		temp = max (smax[13], smax[12]);
		ymax = yrange (max (temp, smax[14]));;
		break;

	case 'a':
		strcpy (linea, "Swapping Activity vs Hour");
		strcpy (lineb, "+: swapin/min   *: swapout/min");
		iplot = 2;
		item[0] = 10;
		item[1] = 11;
		ymax = yrange (max (smax[10], smax[11]));
		break;

	case 'f':
		strcpy (linea, "File Access Activity  vs Hour");
		strcpy (lineb, "+: iget/sec   *: namei/sec   =: dirblk/sec");
		iplot = 3;
		item[0] = 15;
		item[1] = 16;
		item[2] = 17;
		temp = max (smax[15], smax[16]);
		ymax = yrange (max (temp, smax[17]));
		break;

	default:
		return(0);
	}
return(1);
}



float	yrange(ff)
float	ff;
{
static float	ylimit[] = {1.0, 1.5, 2.5, 4.0, 6.0, 10.0};
float	div = 1;
int	i = 0;
if (ff <= 0.)	return(0);
while (ff/div > 10.)	div *= 10.;
while (ff/div < 1.)	div /= 10.;
while ((ff/div) > ylimit[i])	i++;
return (ylimit[i] * div);
}


plotit()
{
/*	plotit() requires these input global variables
	hs, he, hms_s, hms_e
	ymin, ymax
	term
	date
	linea
	lineb
	ip, iplot, item[0], item[1], item[2]
							*/
FILE	*pipe, *popen();
static	char	pchar[] = {'+', '*', '='};
char	cmd[200], tempfile[20];
int	i, j;

sprintf (tempfile, "/tmp/sag%d", getpid());

/*	Construct graph commands for left edge labels		*/

sprintf(cmd, "graph -x %.2f %.2f -y %.2f %.2f -r 0.0 -h .8 -u 0.2 -g 0 -b > %s",
	hs, he, ymin, ymax, tempfile);
fflush (stdout);
if (pipe = popen (cmd, "w"))
	{
	fprintf(pipe, "%.2f %.2f \"%4.0f\"\n%.2f %.2f \"%4.0f\"\n",
		hs, ymax, ymax,
		hs, ymin, ymin);
	pclose (pipe);
	}

/*	Construct graph commands for bottom labels and title	*/

sprintf(cmd, "graph -x %.2f %.2f -y %.2f %.2f -r 0.1  -h .8 -u 0.07 -g 0 -b -s >> %s",
	hs, he, ymin, ymax, tempfile);
fflush (stdout);
if (pipe = popen (cmd, "w"))
	{
	fprintf(pipe, "%.2f %.2f \"\b%s\"\n%.2f %.2f \"%s\"\n",
		hs, (.88*ymin+.12*ymax), hms_s,
		he, (.88*ymin+.12*ymax), hms_e);
	fprintf(pipe, "%.2f %.2f \"%s\"\n",
		(.9*hs+.1*he), (.06*ymax+.94*ymin), date);
	fprintf(pipe, "%.2f %.2f \"%s\"\n",
		(.9*hs+.1*he), (.03*ymax+.97*ymin), linea);
	fprintf(pipe, "%.2f %.2f \"%s\"\n",
		(.9*hs+.1*he), ymin, lineb);
	pclose (pipe);
	}

/*	Construct graph commands for plotting, iplot cases	*/

for (i=0; i<iplot; i++)
{
sprintf(cmd, "graph -x %.2f %.2f -y %.2f %.2f -r 0.1 -h .8 -u 0.2 -g %d -s -c \"%c\" >> %s",
	hs, he, ymin, ymax, ((i == 0) ? 1 : 0), pchar[i], tempfile);
fflush (stdout);
if (pipe = popen (cmd, "w"))
	{
	for (j=0; j<ip; j++)
		fprintf (pipe, "%.2f %.2f\n",
			s[j][0], s[j][item[i]]);
	pclose (pipe);
	}
}
sprintf (cmd, "tplot %s < %s", term, tempfile);
system (cmd);
sprintf (cmd, "rm %s", tempfile);
system (cmd);
}
