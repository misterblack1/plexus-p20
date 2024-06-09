#
/*
 *
 *	UNIX debugger
 *
 */

#include "defs.h"


MSG		BADNAM;
#ifdef OVKRNL
MSG		DIFMAG;
#else
MSG		BADMAG;
#endif

MAP		txtmap;
MAP		datmap;
SYMSLAVE	*symvec;
INT		wtflag;
INT		fcor;
INT		fsym;
L_INT		maxfile;
L_INT		maxstor;
L_INT		txtsiz;
L_INT		datsiz;
L_INT		datbas;
L_INT		stksiz;
STRING		errflg;
INT		magic;
L_INT		symbas;
L_INT		symnum;
L_INT		entrypt;
#ifdef OVKRNL
L_INT		var[];

long tell();
#endif
INT		argcount;
INT		signo;
char		corhdr[ctob(USIZE)];
POS		*endhdr = &corhdr[ctob(USIZE)];

STRING		symfil =	"a.out";
STRING		corfil = "core";

#ifdef OVKRNL
OVLVEC		ovlseg;
L_INT		ovlsiz;
L_INT		ovloff[8];
#endif

#define TXTHDRSIZ	(sizeof(txthdr))
#ifdef OVKRNL
#define OVLHDRSIZ	(sizeof(ovlseg))
#endif

setsym()
{
	INT		relflg;
	INT		symval, symflg;
	SYMSLAVE	*symptr;
	SYMPTR		symp;
	TXTHDR		txthdr;
int i;
int j;

	fsym=getfile(symfil,1);
	txtmap.ufd=fsym;
	IF read(fsym, txthdr, TXTHDRSIZ)==TXTHDRSIZ
	THEN	magic=txthdr[0];
#ifdef OVKRNL
		txtsiz = txthdr[1];
		datsiz = txthdr[2];
		symnum = txthdr[4]/SYMTABSIZ;
		entrypt = txthdr[5];
		relflg = txthdr[7];
		symbas = txtsiz+datsiz;
		switch (magic)
		{
			INT ovly;

			case 0407:
				txtmap.b1 = txtmap.b2 = 0;
				txtmap.e1 = txtmap.e2 = txtsiz+datsiz;
				txtmap.f1 = txtmap.f2 = TXTHDRSIZ;
				break;

			case 0410:
				txtmap.b1 = 0;
				txtmap.e1 = txtsiz;
				txtmap.f1 = TXTHDRSIZ;
				txtmap.b2 = round(txtsiz, TXTRNDSIZ);
				txtmap.e2 = txtmap.b2+datsiz;
				txtmap.f2 = txtsiz+TXTHDRSIZ;
				break;

			case 0405:
			case 0411:
				txtmap.b1 = 0;
				txtmap.e1 = txtsiz;
				txtmap.f1 = TXTHDRSIZ;
				txtmap.b2 = 0;
				txtmap.e2 = datsiz;
				txtmap.f2 = txtsiz+TXTHDRSIZ;
				break;

			case 0430:
			case 0431:
				IF read(fsym, &ovlseg, OVLHDRSIZ) == OVLHDRSIZ
				THEN	txtmap.b1 = 0;
					txtmap.e1 = txtsiz;
					txtmap.f1 = TXTHDRSIZ+OVLHDRSIZ;
				/*	txtmap.bo = round(txtsiz, TXTRNDSIZ); */
					txtmap.bo = txtsiz; 
					txtmap.eo = 0;
					txtmap.fo = 0;
					FOR ovly = 0; ovly < 7; ovly++
					DO	ovloff[ovly] = ovlsiz + txtsiz 
							+TXTHDRSIZ+OVLHDRSIZ;
						ovlsiz += ovlseg.ov[ovly];
					OD
					IF magic == 0430
					THEN	txtmap.b2 =
					 /* round(txtmap.bo+(long)ovlseg.max,
							 TXTRNDSIZ); */
					    txtmap.bo+(long)ovlseg.max; 
					ELSE	txtmap.b2 = 0;
					FI
					txtmap.f2 = TXTHDRSIZ+OVLHDRSIZ+txtsiz+ovlsiz;
					symbas += ovlsiz+OVLHDRSIZ;
					txtmap.e2 = txtmap.b2 + datsiz;
					break;
				FI

			default:	magic = 0;
		}
		datbas = txtmap.b2;
#else
		IF magic!=0411 ANDF magic!=0410 ANDF magic!=0407 ANDF
			 magic!=0405 THEN	magic=0;
		ELSE	symnum=txthdr[4]/SYMTABSIZ;
			txtsiz=txthdr[1];
			datsiz=txthdr[2];
			symbas=txtsiz+datsiz;
			txtmap.b1=0;
			txtmap.e1=(magic==0407?symbas:txtsiz);
			txtmap.f1 = TXTHDRSIZ;
			txtmap.b2=datbas=(magic==0410?round(txtsiz,TXTRNDSIZ):0);
			txtmap.e2=txtmap.b2+(magic==0407?symbas:datsiz);
			txtmap.f2 = TXTHDRSIZ+(magic==0407?0:txtmap.e1);
			entrypt=txthdr[5];
			relflg=txthdr[7];
#endif
		IF relflg!=1 THEN symbas =<< 1; FI
		symbas += TXTHDRSIZ;

		/* set up symvec */
 		symvec=sbrk(shorten((1+symnum))*sizeof (SYMSLAVE));
 		IF (symptr=symvec)==-1
 		THEN	printf("%s\n",BADNAM);
 			symptr=symvec=sbrk(sizeof (SYMSLAVE));
 		ELSE	symset();
 			WHILE (symp=symget()) ANDF errflg==0
 			DO  symval=symp->symv; symflg=symp->symf;
 			    symptr->valslave=symval;
 			    symptr->typslave=SYMTYPE(symflg);
#ifdef OVKRNL
 /*
 				symptr->ovlslave = symp->ovnumb;
 */
#endif
 			    symptr++;
 			OD
		FI
		symptr->typslave=ESYM;
#ifndef OVKRNL
	    FI
#endif
	FI
	IF magic==0 THEN txtmap.e1=maxfile; FI
}

setcor()
{
	fcor=getfile(corfil,2);
	datmap.ufd=fcor;
	IF read(fcor, corhdr, ctob(USIZE))==ctob(USIZE)
	THEN	txtsiz = ctob(corhdr->u_tsize);
		datsiz = ctob(corhdr->u_dsize);
		stksiz = ctob(corhdr->u_ssize);
#ifdef OVKRNL
		datmap.f1 = ctob(USIZE);
		datmap.b2 = maxstor-stksiz;
		datmap.e2 = maxstor;
		switch (magic)
		{
			INT ovly;

			case 0407:
				datmap.b1 = 0;
				datmap.e1 = txtsiz+datsiz;
				datmap.f2 = ctob(USIZE)+txtsiz+datsiz;
				break;

			case 0410:
				datmap.b1 = round(txtsiz, TXTRNDSIZ);
				datmap.e1 = datmap.b1+datsiz;
				datmap.f2 = datsiz+ctob(USIZE);
				break;

			case 0405:
			case 0411:
			case 0431:
				datmap.b1 = 0;
				datmap.e1 = datsiz;
				datmap.f2 = datsiz+ctob(USIZE);
				break;

			case 0430:
				datmap.b1 = round(round(txtsiz,TXTRNDSIZ)+ovlseg.max,
						TXTRNDSIZ);
				datmap.e1 = datmap.b1+datsiz;
				datmap.f2 = datsiz+ctob(USIZE);
				break;

			default:
				magic = 0;
				datmap.b1 = 0;
				datmap.e1 = maxstor;
				datmap.f1 = 0;
				datmap.f2 = 0;
		}
		datbas = datmap.b1;
#else
		datmap.b1 = datbas = (magic==0410?round(txtsiz,TXTRNDSIZ):0);
		datmap.e1=(magic==0407?txtsiz:datmap.b1)+datsiz;
		datmap.f1 = ctob(USIZE);
		datmap.b2 = maxstor-stksiz;
		datmap.e2 = maxstor;
		datmap.f2 = ctob(USIZE)+(magic==0410?datsiz:datmap.e1);
#endif
		IF magic ANDF magic!=corhdr[0].u_exdata.ux_mag
#ifdef OVKRNL
		THEN	printf("%s\n",DIFMAG);
/*-------------------
		There is no way to execute the kernel. Also kernel doesn't have
		user structure to hold current overlay id. So with regrets must
		comment out this code segment.
		ELIF ovlsiz != 0L
		THEN	var[VARC] = (long)
			    ((struct user *)corhdr)->u_ovdata.uo_curov;
-------------------*/

#else
		THEN	printf("%s\n",BADMAG);
#endif
		FI
	ELSE	datmap.e1 = maxfile;
	FI
}

create(f)
STRING	f;
{	int fd;
	IF (fd=creat(f,0644))>=0
	THEN close(fd); return(open(f,wtflag));
	ELSE return(-1);
	FI
}

getfile(filnam,cnt)
STRING	filnam;
{
	REG INT		fsym;

	IF !eqstr("-",filnam)
	THEN	fsym=open(filnam,wtflag);
		IF fsym<0 ANDF argcount>cnt
		THEN	IF wtflag
			THEN	fsym=create(filnam);
			FI
			IF fsym<0
			THEN printf("cannot open `%s'\n", filnam);
			FI
		FI
	ELSE	fsym = -1;
	FI
	return(fsym);
}
