int	mkfault;
int	stdout 1;
char printbuf[255]; char *printptr printbuf, *digitptr;
printc(c)
char c;
{
	if( (*printptr++ = c)=='\n' || c==0
	){ if( mkfault==0 ){ write(stdout,printbuf,printptr-printbuf); }
	     printptr=printbuf;
	}
}
charpos()
{	return(printptr-printbuf);
}
flushout()
{	if( printptr!=printbuf
	){ printc(0);
	}
}
printf(fmat,a1)
char *fmat, **a1;
{
	char *fptr, *s; int *vptr; long *dptr; double *rptr;
	double real, rnd;
	int x, decpt, n; long lx;
	int width, prec; char c, adj; char flush;
	char digits[64];
	fptr=fmat; vptr = &a1;
	while( c = *fptr++
	){  if( c!='%'
	    ){ printc(c);
	    } else { if( *fptr=='-' ){ adj='l'; fptr++; } else { adj='r'; }
		 width=convert(&fptr);
		 if( *fptr=='.' ){ fptr++; prec=convert(&fptr); } else { prec = -1; }
		 digitptr=digits;
		 dptr=rptr=vptr; lx = *dptr; x = *vptr++;
		 s=0; flush=0;
		 switch (c = *fptr++) {
		    case 'n':
			flush++; break;
		    case 'd':
		    case 'u':
			printnum(x,c,10); break;
		    case 'o':
			printoct(0,x,0); break;
		    case 'q':
			lx=x; printoct(lx,-1); break;
		    case 'x':
			printdbl(0,x,c,16); break;
		    case 'Y':
			printdate(lx); vptr++; break;
		    case 'D':
		    case 'U':
			printdbl(lx,c,10); vptr++; break;
		    case 'O':
			printoct(lx,0); vptr++; break;
		    case 'Q':
			printoct(lx,-1); vptr++; break;
		    case 'X':
			printdbl(lx,'x',16); vptr++; break;
		    case 'c':
			printc(x); break;
		    case 's':
			s=x; break;
		    case 'f':
		    case 'F':
			vptr =+ 3;
			rnd=1.0;
			for(n=prec;n>=0;n--)
				rnd =* 10;
			real = *rptr + (5 / rnd);
			s=ecvt(real, 8, &decpt, &n);
			if( n ){ *digitptr++='-'; }
			*digitptr++ = (decpt<=0 ? '0' : *s++);
			while( *s && --decpt>0 ){ *digitptr++ = *s++; }
			*digitptr++ = '.';
			while( *s && prec-- ){ *digitptr++ = (++decpt<0?'0':*s++); }
			*digitptr++=0;
			s=0; prec = -1; break;
		    case 'm':
			vptr--; break;
		    case 'M':
			width=x; break;
		    case 'T':
		    case 't':
			if( c=='T'
			){ width=x;
			} else { vptr--;
			}
			if( width
			){ width =- charpos()%width;
			}
			break;
		    default:
			printc(c); vptr--;
		}
		if( s==0
		){ *digitptr=0; s=digits;
		}
		n=length(s);
		n=(prec<n && prec>=0 ? prec : n);
		width =- n;
		if( adj=='r'
		){ while( width-- > 0
		     ){ printc(' '); }
		}
		while( n-- ){ printc(*s++); }
		while( width-- > 0 ){ printc(' '); }
		digitptr=digits;
		if( flush ){ flushout(); }
	    }
	}
}
printdate(tvec)
long tvec;
{
	char * timeptr; register int i;
	timeptr = ctime(&tvec);
	for( i=20; i<24; i++ ){ *digitptr++ = *(timeptr+i); }
	for( i=3; i<19; i++ ){ *digitptr++ = *(timeptr+i); }
} 
prints(s)
char *s;
{	printf("%s",s);
}
convert(cp)
char **cp;
{	char c; int n;
	n=0;
	while( ((c = *(*cp)++)>='0') && (c<='9') ){ n=n*10+c-'0'; }
	(*cp)--;
	return(n);
}
printnum(n,fmat,base)
{	char k; int digs[15]; int *dptr;
	dptr=digs;
	if( n<0 && fmat=='d' ){ n = -n; *digitptr++ = '-'; }
	while( n
	){  n = ((unsigned) n) / base;
	    *dptr++ = ((unsigned) n) % base;
	}
	if( dptr==digs ){ *dptr++=0; }
	while( dptr!=digs
	){  k = *--dptr;
	    *digitptr++ = (k+(k<=9 ? '0' : 'a'-10));
	}
}
printoct(o,s) long o; int s;
{
	int i;
	long po;
	char digs[12];
	po = o;
	if( s
	){ if( po<0
	     ){ po = -po; *digitptr++='-';
	     } else { if( s>0 ){ *digitptr++='+'; }
	     }
	}
	for( i=0;i<=11;i++
	){ digs[i] = po&7; po =>> 3; }
	digs[10] =& 03; digs[11]=0;
	for( i=11;i>=0;i--
	){ if( digs[i] ){ break; } }
	for( i++;i>=0;i--
	){ *digitptr++=digs[i]+'0'; }
}
printdbl(lx,ly,fmat,base)
char *lx, *ly; char fmat; int base;
{	int digs[20]; int *dptr; char k;
	double f ,g; long q;
	dptr=digs;
	if( fmat!='D'
	){	f= *lx; f =* 	(((long) 1) << 16 | ((long) (unsigned) 0)); f =+ *ly;
		if( fmat=='x' ){ *digitptr++='#'; }
	} else {	f=	(((long) lx) << 16 | ((long) (unsigned) ly));
		if( f<0 ){ *digitptr++='-'; f = -f; }
	}
	while( f
	){  q=f/base; g=q;
	    *dptr++ = f-g*base;
	    f=q;
	}
	if( dptr==digs ){ *dptr++=0; }
	while( dptr!=digs
	){  k = *--dptr;
	    *digitptr++ = (k+(k<=9 ? '0' : 'a'-10));
	}
}
length(s)
char *s;
{
	register char *t;
	t=s;
	while( *t++ );
	return(t-s-1);
}
