#
/**********************************************************************/
/*   Module pass1 contains the controlling routines for the first     */
/*        pass of the assembler.  All global data is declared in this */
/*        module.  Stub routines not yet written also reside in this  */
/*        module.                                                     */
/**********************************************************************/


#include <a.out.h>
#include <stdio.h>
#include "common.decs"
#include "error.decs"
#include "ifile.decs"
#include "mask.decs"
#include "symbol.decs"
#include "symtab.decs"
#include "token.decs"
#include "types.decs"
	
#include "extern.decs"
#include "extern1.decs"


/**********************************************************************/
/*   Pass1 simply calls function "statement" until an end of file is  */
/*        encountered.  Initially the IFILE and SOURCE files are      */
/*        opened.  At termination, they are closed.                   */
/**********************************************************************/
 
pass1 () {

	if (openpass1() != SUCCESS) abort();

	gchar();
	scan();
	while ((tok & BYTEMASK) != EOFV)
		statement();

	closepass1();
}


/**********************************************************************/
/*   Openpass1 opens files and sets up for pass1 to start processing. */
/**********************************************************************/

openpass1 () {

	register int val;

	val = getpid();
	extenptr = &tmpfile[11];
	while (val > 0) {
		*extenptr++ = (val & 0X000F) + 'a';
		val >>= 4;
	}

	*++extenptr = 0;
	--extenptr;

	if (ifile == 0) {
		ifile = tmpfile;
		*extenptr = IFILE;
	}

	if ((ifilebuf = fopen(ifile,"w+")) == NULL) {
		faterror(NOCREATE,ifile);
		return (FAILURE);
	}
	return (SUCCESS);
}


/**********************************************************************/
/*   Closepass1 closes pass1 files and gets ready for pass2 to start. */
/**********************************************************************/

closepass1 () {

	locflag = FALSE;
	iputlc(0);
	updatelc(sc);

	if (textlc%2 == 1) {
		textlc++;
		textbyte = TRUE;
	}
	if (datalc%2 == 1)
		datalc++;
	if (bsslc%2 == 1)
		bsslc++;

	iputeols();
	iputtag(ENDMODULE);

	*extenptr = IFILE;
	fclose(ifilebuf);

	return;
}


/**********************************************************************/
/*   Statement determines which type of statement has been            */
/*        encountered, and calls the appropriate function to handle   */
/*        that statement.                                             */
/**********************************************************************/
 
statement () {
 
	switch (tok) {

	case OPCODE:
	case XOPCODE:
	case CONTROL:
	case VAR:
	case DOTV:
	case DOTDOTV:
		labelstatement();
		break;

	case CONSTVAR:
	case NUM:
	case STR:
		iputeols();
		if (zcc == TRUE)
			labelstatement();
		else {
			error1(ILLSTAT);
			scan();
		}
		break;

	case NEWLINEV:
	case SEMIV:
		scan();
		break;

	case EOFV:
		break;
 
	default:
		declaration();
		break;

	}
}


/**********************************************************************/
/*   Init serves the purpose of initializing the hash table           */
/*        symbol table, section table, and some global variables.     */
/*        This routine needs to be called once for every module       */
/*        assembled.                                                  */
/**********************************************************************/


init () {

	register int i;
	register int lastopcode;
	register int pos;
	register char *ptr1;
	register int size;


	firstid = 0xffff;
	freespace = 2;
	freestorage = malloc(freespace);

	endopbuffer = &opbuffer[OPBUFFERSIZE - 1];
	endoprbuf = &oprbuf[MAXOPCOUNT - 1];
	tokstr = &tokbuf[0];

	for (i = 0; i < HASHTABSIZE; i++)
		hashtab[i] = EMPTY;

	curid = 2;
	symptr = getsym(curid);
	lastopcode = 0;

	while ((symptr->STYPE & BYTEMASK) != ENDSYMTAB) {
		bufptr = tokstr;
		ptr1 = &symptr->SIZE;
		size = *ptr1 + 1;
		for (i=0; i<size; i++)
			*bufptr++ = *ptr1++;
		if ((pos = lookup()) == TRUE) {
			faterror(BUG);
		}

		symptr = getsym(curid);

		if (symptr->STYPE == OPCODE) {
			symptr->SLOCATION = lastopcode;
			lastopcode += symptr->STYPEPTR;
		}

		hashtab[pos] = curid;
		curid += sizeof (SYMBOL) + symptr->SIZE - 1;
		if (curid%2 != 0) curid++;
		symptr = getsym(curid);
	}

	nextpos = firstid = curid;

	sc = N_TEXT;
	lc = 0;
	split = FALSE;
}


/*****************************************************************/
/*   Scanto ignores tokens until either an EOF or the token      */
/*        passed as an argument is encountered.                  */
/*****************************************************************/

scanto (token)

char token;

{

	while ((tok != token) && (tok != EOFV))
		scan();
}


/*****************************************************************/
/*   Recover ignores tokens until a reasonable spot is found     */
/*        to resume scanning.                                    */
/*****************************************************************/

recover () {

	switch (tok) {

	case SEMIV:
	case NEWLINEV:
		scan();
		break;
 
	case CONTROL:
	case OPCODE:
	case XOPCODE:
	case EOFV:
		break;

	default:
		scan();
	}
}


/**********************************************************************/
/*   The following functions are only stubs and need to be completed  */
/*        at some later date.                                         */
/**********************************************************************/

/**********************************************************************/
/*   Controline processes a AS control line and performs the          */
/*        necessary actions.                                          */
/**********************************************************************/

controline () {

	register int control;
	register int i;
	register unsigned labelid;
	register int retval;
	register int siz;
	register long val;

	control = slocation;
	iputeols();
	scan();

	switch (control) {

	case CBYTEV:
/* next line necessary ? */
		forcelc();
		if (sc == N_BSS)
			error1(BSSINIT);
		getinitval(BVAL);
		iputlc(1);
		while (tok == COMMAV) {
			scan();
			getinitval(BVAL);
			iputlc(1);
		}
		break;

	case CWORDV:
/* even() sted nextl line ok ? */
		forcelc();
		if (sc == N_BSS)
			error1(BSSINIT);
		getinitval(WVAL);
		iputlc(2);
		while (tok == COMMAV) {
			scan();
			getinitval(WVAL);
			iputlc(2);
		}
		break;

	case CLONGV:
/* even() sted next line ok? */
		forcelc();
		if (sc == N_BSS)
			error1(BSSINIT);
		getinitval(LVAL);
		iputlc(4);
		while (tok == COMMAV) {
			scan();
			getinitval(WVAL);
			iputlc(4);
		}
		break;

	case CDATAV:
		locflag = FALSE;
		iputlc(0);
		updatelc(N_DATA);
		if (sc != N_DATA) {
			iputbyte(N_DATA);
			lc = datalc;
			iputword(lc);
			sc = N_DATA;
		}
		break;

	case CTEXTV:
		locflag = FALSE;
		iputlc(0);
		updatelc(N_TEXT);
		if (sc != N_TEXT) {
			iputbyte(N_TEXT);
			lc = textlc;
			iputword(lc);
			sc = N_TEXT;
		}
		break;

	case CASCIIV:
	case CASCIIZV:
		if (tok != STR)
			error1(ILLCEXPR);
		else {
			forcelc();
			iputbyte(ASCII);
			siz = *tokstr;
			if (control == CASCIIZV)
				siz++;
			iputbyte(siz);
			for (i=1; i<=*tokstr; i++) {
				iputbyte(tokbuf[i]);
				iputlc(1);
			}
			if (control == CASCIIZV) {
				iputbyte(0);
				iputlc(1);
			}
			scan();
		}
		break;

	case CBSSV:
		locflag = FALSE;
		iputlc(0);
		updatelc(N_BSS);
		if (sc != N_BSS) {
			iputbyte(N_BSS);
			lc = bsslc;
			iputword(lc);
			sc = N_BSS;
		}
		break;

	case CIFV:
		ifcount++;

		retval = constexpr(&val);

		if (retval != N_ABS) {
			error1(ILLCEXPR);
			break;
		}

		i = ifcount;

		if (!LOWORD(val)) {
			for (;;) {
				while (tok != CONTROL) {
					noerror = TRUE;
					scan();
					noerror = FALSE;
					if (tok == EOFV)
						return;
				}
				if (slocation == CENDIFV) {
					scan();
					if (ifcount-- == i)
						break;
				}
				else if (slocation == CIFV) {
					scan();
					ifcount++;
				}
				else
					scan();
			}
		}
		break;

	case CENDIFV:
		if (ifcount-- <= 0)
			error1(ILLSTAT);
		break;

	case CLISTOFFV:
		iputtag(LISTOFF);
		break;

	case CLISTONV:
		iputtag(LISTON);
		break;

	case CORGV:
		if (constexpr(&val) != N_ABS)
			error1(ILLCEXPR);
		if (lc <= LOWORD(val))
			lc = LOWORD(val);
		else
			error1(DOTDECREMENT);
		break;

	case CGLOBLV:
		for (;;) {
			if (tok != VAR)
				break;
			symptr = getsym(curid);
			symptr->SECTNUM |= N_EXT;
			scan();		/* Scan past symbol */
			if (tok != COMMAV)
				break;
			scan();		/* Scan past comma */
		}
		break;

	case CCOMMV:
		labelid = curid;

		if (tok != VAR) {
			error1(ILLSTAT);
			break;
		}

		scan();			/* Scan past symbol */

		if (tok != COMMAV) {
			error1(ILLSTAT);
			break;
		}
		scan();			/* Scan past comma */

		retval = constexpr(&val);

		if ((retval & ~N_EXT) == N_UNDF)
			error1(ILLCEXPR);

		symptr = getsym(labelid);

		if ((symptr->SECTNUM & ~N_EXT) == N_UNDF)
			if (val > symptr->SLOCATION)
				symptr->SLOCATION = val;
		symptr->SECTNUM |= N_EXT;
		break;

	case CPAGEV:
		iputtag(PAGE);
		break;

	case CEVENV:
/* even sted all this ? */
		if (lc%2 == 1) {
			forcelc();
			iputbyte(BVAL);
			iputbval(LDBYTE,0);
			iputlc(1);
		}
		break;

	default:
		error1(BUG);
		break;
	}
}


/**********************************************************************/
/*   Xopcode processes an extended opcode and generates the           */
/*        appropriate intermediate file code.                         */
/**********************************************************************/

xopcode () {

	error1(ILLINST);
	scanto(NEWLINEV);
	return;

}


/**********************************************************************/
/*   Declaration parses all declaration statements, including         */
/*        initializations, and outputs the appropriate information    */
/*        to the IFILE.                                               */
/**********************************************************************/

declaration () {

	register unsigned symtype;

	/* Get out of here for now */

	eolerror(ILLSTAT);
	scan();

	/* Declaration really starts here */
	symtype = tok;

	switch (symtype) {

	case SHORTV:
	case UNSIGNEDV:
	case LONGV:
		scan();
		if (tok == INTV)
			scan();		/* Scan past "int" */
		break;

	case CHARV:
	case INTV:
	case FLOATV:
	case DOUBLEV:
		scan();
		break;

	case STRUCTV :
		scan();				/* Scan past "struct" */
		symtype = recordec();
		break;

	case TYPEVAR:
		symtype = curid;
		scan();
		break;

	default :
		error1(ILLSTAT);
		scan();
	}

	declist(symtype);
}


/**********************************************************************/
/*   Declist handles declaration lists.  It assumes the type of the   */
/*        declaration has already been parsed and that the size has   */
/*        been determined and is passed as "size".  An asterisk       */
/*        preceding a name will be interpretted as a pointer and a    */
/*        word will be allocated.  A left bracket following a name    */
/*        will signify an array, and the subscripts are then parsed   */
/*        to determine the size.                                      */
/**********************************************************************/

declist (symtype)

register unsigned symtype;

{
	register int size;
	register long val;
	register int arraysize;

	size = getsize(symtype);

	while (tok != SEMIV) {

		pointer = FALSE;

		while (tok == STARV) {
			pointer = TRUE;
			scan();   		/* Scan past "*" */
		}

		if (tok != VAR)
			break;

		symptr = getsym(curid);

		/*
		if (symptr->SFLAGS & DEFMASK) {

			if (checkdef() == TRUE) break;

			symptr->SFLAGS |= DEFMASK;
			symptr->SECTNUM = sc;
			symptr->SLOCATION = lc;
		}
		*/

		scan();				/* Scan past identifier */

		if (tok != LBRACKV)
			if (pointer = TRUE)
			iputlc(POINTERSIZE);
			else
			iputlc(size);

		else {
			arraysize = 1;
			scan();   		/* Scan past left bracket */
			if (tok == RBRACKV) {
			iputlc(POINTERSIZE);
			}
			else {
				while (tok != RBRACKV) {
					if (constexpr(&val) == UNDEF)
						error1(ILLCEXPR);
					arraysize *= val;
					if (tok != COMMAV)
						break;
					scan();		/* Scan past comma */
				}
				scan();			/* Scan past "]" */
				if (pointer == TRUE)
				iputlc(POINTERSIZE);
				else
				iputlc(arraysize);
			}

			if (tok != COMMAV)
				break;

			scan();			/* Scan past "," */
		}
	}
}


/************************************************************************/
/*   Recordec handles record declarations.				*/
/*									*/
/*	Currently, it is a mere stub.					*/
/************************************************************************/

recordec () {

	;
}
getsize () {
}

