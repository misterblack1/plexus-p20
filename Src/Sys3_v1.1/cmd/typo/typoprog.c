/*	typo - find potential spelling mistakes
 *
 */

char *xxxvers = "@(#)typoprog.src	1.2";

#include <stdio.h>
#include "pinhead.h"
#include <signal.h>

# define CHARL 50
# define CHARLM CHARL - 2
int	sqflg	= 0;
int	status;
int	sn = -1;
char	hole[26];
char	w2006[CHARL];
int	flg;
char	realwd[CHARL];
char	*wd = {
	&realwd[1]};
int	neng;
struct {
	int	tabs[2];
	int	tab1[26];
	int	tab2[730];
	char	tab3[19684];
} table;
int	logtab[256];
float	inctab[256];
char	nwd[CHARL];
int	tot;
int	wtot;
FILE	*file[3];
char	*name[3];
char	*wdval();
char	*nwdval();
char	*mktemp();

main(argc,argv) int argc; 
char *argv[]; 
{
	FILE *salt;
	int arg,sw,er,c;
	register char *sk, *sl, *sm;
	char *si, *sj;
	float junk;
	int unl();
	int ii;
	float log(), exp(), pow();

	inctab[0] = 1;
	logtab[0] = -10;
	for(ii=1; ii<256; ii++){
		inctab[ii] = exp(-ii/30.497);
		logtab[ii] = log(30.*pow(1.0333,ii+0.) - 30.) + .5;
	}
	logtab[1] = -10;

	arg = 1;
	while(argc>1 && argv[arg][0] == '-') {
		switch(argv[arg][1]) {
		default:
			printf("Unrecognizable argument: %c\n",argv[arg][1]);
			exit(0);
		case 0:
		case 'n':
			neng++;
			break;
		}
		arg++;
		--argc;
	}
	if(!neng) {
		salt = fopen("/usr/lib/salt","r");
		er = fread((char *)&table, 1, sizeof(table), salt);
		if(er != sizeof(table))err("read salt");
		fclose(salt);
	}
	name[0] = mktemp("/usr/tmp/ttmpaaXXXXX");
	name[1] = mktemp("/usr/tmp/ttmpbaXXXXX");
	name[2] = mktemp("/usr/tmp/ttmpcaXXXXX");
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, unl);
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		signal(SIGHUP, unl);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, unl);
	file[0] = fopen(name[0], "w");
	if(argc == 1) {
		argc = 2; 
		arg = 0;
	}
	while(--argc){
		if(arg == 0){
			file[2] = stdin;
		}
		else{
			file[2] = fopen(argv[arg++],"r");
			if(file[2] == 0)err("open input file");
		}
		while((sj = wdval(2)) != 0){
			fprintf(file[0],"%s",nwd);
			sk = realwd;
			sl = wd;
			sm = &wd[1];
			if(inctab[table.tabs[0]] > (junk=rand()/32768.))
				table.tabs[0]++;
			while(sm != sj){
				c = 27 * (*sk++) + (*sl++);
				if(inctab[table.tab2[c]] > junk) table.tab2[c]++;
				c = 27*c + (*sm++);
				if(inctab[table.tab3[c]] > junk)
					table.tab3[c]++;
			}
			c = 27 * (*sk) + (*sl);
			if(inctab[table.tab2[c]] > junk) table.tab2[c]++;
		}
		if(file[2] != stdin) fclose(file[2]);
	}
	fclose(file[0]);
	sw = fork();
	if(sw == 0){
		execl("/bin/sort", "sort", "-u", "+0f", "+0", "-o", name[2], name[0], 0);
		err("sort"); 
	}
	if(sw == -1)err("fork");
	er = wait(&status);
	if(er != sw)err("probs");
	file[0] = fopen(name[0],"w");
	if(file[0] == 0)err("creat tmp");
	file[1] = fopen("/usr/lib/sq2006","r");
	if(file[1] == 0)err("open w2006");
	getsq(1);
	file[2] = fopen(name[2],"r");
	if(file[2] == 0)err("open tmp");

	while(1){
		sl = w2006;
		sm = nwd;
		for(sk = wd;((c = *sm++ = *sk = getc(file[2])) != '\n');sk++){
			if(c == EOF)goto done;
			if(c >= 'A' && c <= 'Z')*sk += ' ';
		}
		*sm++ = '\0';
		for(si = wd; si != sk; si++){
cont:
			if(*si < *sl){
				break;
			}
			if(*si > *sl){
				if(getsq(1) == -1){
					fprintf(file[0],"%s",nwd);
					for(sk=wd;((c = *sk =getc(file[2]))!= EOF);sk++){
						fprintf(file[0],"%s",wd);
						sk = wd;
					}
					goto done;
				}
				si = wd;
				sl = w2006;
				goto cont;
			}
			sl++;
		}
		if(*sl != '\n')fprintf(file[0],"%s",nwd);
	}
done:
	fclose(file[2]); 
	unlink(name[2]);
	fclose(file[1]);
	fclose(file[0]);
	file[1] = fopen(name[0],"r");
	if(file[1] == 0)err("open tmp ");
	while((sj = nwdval(1)) != 0){
		wtot = 0;
		flg = 0;
		sk = realwd;
		sl = wd;
		sm = &wd[1];
		while(sm != sj){
			tot = 0;
			c = (*sk++)*27 + (*sl++);
			tot += (logtab[table.tab2[c]]+logtab[table.tab2[(*sk)*27+(*sl)]]);
			tot >>= 1;
			c = c*27 + (*sm++);
			tot -= logtab[table.tab3[c] & 0377];
			if(tot > wtot) wtot = tot;
		}
		if(wtot < 0) wtot = 0;
		printf("%2d %s",wtot,nwd);
	}
	unl();
}

unl() {
	register j;
	for (j=0; j<2; j++)
		unlink(name[j]);
	exit(0);
}

err(c) char c[];
{
	printf("cannot %s\n",c);
	unl();
}

char *
wdval(wfile) int wfile; 
{
	static let,wflg;
	register char *sj, *sk;
beg:
	sj = wd;
	sk = nwd;
	if(wflg == 1){
		wflg = 0;
		goto st; 
	}
	while((let = getc(file[wfile])) != '\n'){
st:
		if(let == EOF)return(0);
		if(sj >= &wd[CHARLM]){
			while((((let=getc(file[wfile]))>= 'A') && (let <= 'Z'))
			    || ((let >= 'a') && (let <= 'z')));
			goto ret;
		}
		switch(let){
		case '-':
			if((let = getc(file[wfile])) == '\n'){
				while((let = getc(file[wfile])) == '\n')if(let == EOF)return(0);
				goto st; 
			}
			else {
				wflg = 1;
				goto ret; 
			}
		case '\'':
			if(sj == wd)goto beg;
			if(((let=getc(file[wfile])) >= 'A' && let <= 'Z')||
			    (let >= 'a' && let <= 'z')){
				*sk++ = '\'';
				goto st;
			}
			else {
				wflg = 0;
				goto ret;
			}
		default:
			if(let < 'A')goto ret;
			if(let <= 'Z'){ 
				*sj++ = let - 0100;
				*sk++ = let ;
				break; 
			}
			if(let < 'a' || let > 'z')goto ret;
			*sj++ = let - 0140;
			*sk++ = let;
		}
	}

ret:
	if(sj == wd || sj == &wd[1])goto beg;
	*sk++ = '\n';
	*sk = *sj++ = '\0';
	return(sj);
}

char *
nwdval(wfile) int wfile;
{
	register char *sj, *sk;
	register c;
	sj = wd;
	sk = nwd;
	do{
		if(( c = *sk++ = getc(file[wfile])) == EOF)return(0);
		if(c != '\''){
			if(c >= 'a')*sj++ = c - 0140;
			else *sj++ = c - 0100;
		}
	}
	while(c != '\n');
	*sk = *(sj-1) = '\0';
	return(sj);
}
rand(){
	static gorp;
	gorp = (gorp + 625) & 077777;
	return(gorp);
}

getsq(ifile) int ifile;
{
	register char *p, *cp;
	register c;
	int n, f;

	if(sqflg == 0){
		for(c=7; c<16; c++)
			hole[nib[c]-'a'] = c+64-1+26;
		for(n=0; cp=suftab[n]; n++) {
			p = w2006;
			while(*p++ = *cp++)
				;
			p--;
			cp = suftab[n];
			while(p > w2006)
				*cp++ = *--p;
		}
		sqflg = 1;
	}

	f = 0;
	if((c = getn(ifile)) < 0)return(-1);
	if(c < 10) {
		if(c == 9) {
			if((c = getn(ifile)) < 0)return(-1);
			if(c > 2)
				c += 9;
		} 
		else
			c += 3;
	} 
	else {
		f++;
		if(c == 15) {
			if((c = getn(ifile)) < 0)return(-1);
			if(c > 2)
				c += 5;
		} 
		else
			c -= 7;
	}
	p = w2006+c;

chr:
	if((c = getn(ifile)) < 0)return(-1);
	n = nib[c];
	if(n) {
		if(n == '\n')
			goto out;
		*p++ = n;
		goto chr;
	}
	c = c*16 + getn(ifile);
	if(c < 26) {
		if(hole[c] == 0) {
			*p++ = c+'a';
			goto chr;
		}
		c = hole[c];
	}
	cp = suftab[c-26];
	while(*cp)
		*p++ = *cp++;

out:
	w2006[0] |= 040;
	if(f)
		w2006[0] &= ~040;
	*p++ = '\n';
	return(1);
}

getn(ifile) int ifile;
{
	register c;

	c = sn;
	if(c < 0) {
		c = getc(file[ifile]);
		if(c == EOF) return(-1);
		sn = c >> 4;
		return(c&017);
	}
	sn = -1;
	return(c);
}
