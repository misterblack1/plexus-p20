#
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */

#include	"defs.h"
#include	<sys/types.h>
#define DIRSIZ 15
#include	<sys/stat.h>
#include	<sys/dir.h>



/* globals (file name generation)
 *
 * "*" in params matches r.e ".*"
 * "?" in params matches r.e. "."
 * "[...]" in params matches character class
 * "[...a-z...]" in params matches a through z.
 *
 */

PROC VOID	addg();


INT	expand(as,rflg)
	STRING		as;
{
	INT		count, dirf;
	BOOL		dir=0;
	STRING		rescan = 0;
	REG STRING	s, cs;
	ARGPTR		schain = gchain;
	struct direct	entry;
	STATBUF		statb;
	REG INT		i;

	IF trapnote&SIGSET THEN return(0); FI

	s=cs=as; entry.d_name[DIRSIZ-1]=0; /* to end the string */

	/* check for meta chars */
	BEGIN
	   REG BOOL slash, open; slash=0;open=0;
	   REP
		SWITCH *cs++ IN
		case 0:		IF rflg ANDF slash THEN break;
				ELSE return(0);
				FI

		case '/':	slash++;
				open = 0;
				continue;

		case '[':	open++;
				continue;

		case ']':	IF open THEN break FI
				continue;

		case '?':
		case '*':
				cs--;
				break;
		default:	continue;
		ENDSW
		break;
	   PER TRUE DONE
	END

	LOOP	IF cs==s
		THEN	s=nullstr;
			break;
		ELIF *--cs == '/'
		THEN	*cs=0;
			IF s==cs THEN s="/" FI
			break;
		FI
	POOL
	chgquot(s, 0);
	IF stat(*s?s:".",&statb)>=0
	    ANDF (statb.st_mode&S_IFMT)==S_IFDIR
	    ANDF (dirf=open(*s?s:".",0))>0
	THEN	dir++;
	FI
	chgquot(s, 1);
	count=0;
	IF *cs==0 THEN *cs++=0200 FI
	IF dir
	THEN	/* check for rescan */
		REG STRING pm;
		REG STRING rs; rs=cs;

		REP	IF *rs=='/' THEN rescan=rs; *rs=0; gchain=0 FI
		PER	*rs++ DONE

		WHILE read(dirf, &entry, 16) == 16 ANDF (trapnote&SIGSET) == 0
		DO	IF entry.d_ino==0 ORF
			    (*entry.d_name=='.' ANDF *cs!='.')
			THEN	continue;
			FI
/*
 *	Here lies the fix for the "echo * /." problem when
 *	there are files with metacharacters in there names.
 */
			chgquot(entry.d_name, 1);
			IF gmatch(entry.d_name, cs)
			THEN	addg(s,entry.d_name,rescan); count++;
			FI
			chgquot(entry.d_name, 0);
		OD
		close(dirf);

		IF rescan
		THEN	REG ARGPTR	rchain;
			rchain=gchain; gchain=schain;
			IF count
			THEN	count=0;
				WHILE rchain
				DO	count += expand(rchain->argval,1);
					rchain=rchain->argnxt;
				OD
			FI
			*rescan='/';
		FI
	FI

	BEGIN
	   REG CHAR	c;
	   s=as;
	   WHILE c = *s
	   DO	*s++=(c&STRIP?c:'/') OD
	END
	return(count);
}

gmatch(s, p)
	REG STRING	s, p;
{
	REG INT		scc;
	CHAR		c;

	IF scc = *s++
	THEN	IF (scc &= STRIP)==0
		THEN	scc=0200;
		FI
	FI
	SWITCH c = *p++ IN

	    case '[':
		{BOOL ok; INT lc; INT notflag=0;
		ok=0; lc=077777;
		IF *p == '!' THEN notflag=1; p++; FI
		WHILE c = *p++
		DO	IF c==']'
			THEN	return(ok?gmatch(s,p):0);
			ELIF c==MINUS
			THEN	IF notflag
				THEN	IF lc>scc ORF scc>*(p++)
					THEN ok++;
					ELSE return(0)
					FI
				ELSE IF lc<=scc ANDF scc<=(*p++) THEN ok++ FI
				FI
			ELSE	IF notflag
				THEN	IF scc!=(lc=(c&STRIP))
					THEN ok++;
					ELSE return(0)
					FI
				ELSE IF scc==(lc=(c&STRIP)) THEN ok++ FI
				FI
			FI
		OD
		return(0);
		}
	    default:
		IF (c&STRIP)!=scc THEN return(0) FI

	    case '?':
		return(scc?gmatch(s,p):0);

	    case '*':
		IF *p==0 THEN return(1) FI
		--s;
		WHILE *s
		DO  IF gmatch(s++,p) THEN return(1) FI OD
		return(0);

	    case 0:
		return(scc==0);
	ENDSW
}

LOCAL VOID	addg(as1,as2,as3)
	STRING		as1, as2, as3;
{
	REG STRING	s1, s2;
	REG INT		c;

	s2 = locstak()+BYTESPERWORD;

	s1=as1;
	WHILE c = *s1++
	DO	IF (c &= STRIP)==0
		THEN	*s2++='/';
			break;
		FI
		*s2++=c;
	OD
	s1=as2;
	WHILE *s2 = *s1++ DO s2++ OD
	IF s1=as3
	THEN	*s2++='/';
		WHILE *s2++ = *++s1 DONE
	FI
	makearg(endstak(s2));
}

makearg(args)
	REG STRING	args;
{
	((ARGPTR) args)->argnxt=gchain;
	gchain=(ARGPTR) args;
}

chgquot(str, flg)
REG STRING	str;
REG INT		flg;
{
	REG INT i;

	FOR i=0;i<DIRSIZ;i++
	DO
		SWITCH str[i]  IN
		case '\0':
			return;
		case '*':
		case '?':
		case '[': 
		case '*'|0200:
		case '?'|0200:
		case '['|0200: 
			IF flg==0
			THEN
				str[i] &= (~QUOTE);
			ELSE
				str[i] |= QUOTE;
			FI
		ENDSW
	OD
}
