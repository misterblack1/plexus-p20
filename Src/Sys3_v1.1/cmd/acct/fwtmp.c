# include <stdio.h>
# include <utmp.h>

struct	utmp	Ut;

main ( c, v, e )
char	**v, **e;
int	c;
{

	int	iflg,cflg;

	iflg = cflg = 0;

	while ( --c > 0 ){
		if(**++v == '-') while(*++*v) switch(**v){
		case 'c':
			cflg++;
			continue;
		case 'i':
			iflg++;
			continue;
		}
		break;
	}

	for(;;){
		if(iflg){
			if(inp(stdin,&Ut) == EOF)
				break;
		} else {
			if(fread(&Ut,sizeof Ut, 1, stdin) != 1)
				break;
		}
		if(cflg)
			fwrite(&Ut,sizeof Ut, 1, stdout);
		else	printf("%-8.8s %-8.8s %lu %s", Ut.ut_line, Ut.ut_name, Ut.ut_time, ctime(&Ut.ut_time));
	}
	exit ( 0 );
}
inp(file, u)
FILE *file;
register struct utmp *u;
{

	char	buf[BUFSIZ];
	register char *p;
	register  i;

	if(fgets((p = buf), BUFSIZ, file)==NULL)
		return EOF;

	for(i=0; i<8; i++)
		if((u->ut_line[i] = *p++)==' ')
			u->ut_line[i] = '\0';
	p++;
	for(i=0; i<8; i++)
		if((u->ut_name[i] = *p++)==' ')
			u->ut_name[i] = '\0';
	p++;
	sscanf(p,"%lu", &u->ut_time);
	return((unsigned)u);
}
