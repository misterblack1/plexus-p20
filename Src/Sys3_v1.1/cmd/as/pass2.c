#
/*        and listing file.  Tags in the IFILE are scanned and either */
/*        processed by this module or sent to other modules for       */
/*        processing.  List file primitive routines are also          */
/*        included in this module.  Actual output is done by the      */
/*        routines in the "object.c" module.                          */
/**********************************************************************/


#include "mode.decs"
#include <stdio.h>
#include <a.out.h>
#include <stdio.h>
#include "common.decs"
#include "error.decs"
#include "ifile.decs"
#include "mask.decs"
#include "symtab.decs"
#include "types.decs"

#include "extern.decs"
#include "extern2.decs"


char *finalmess = "\n%d errors encountered\n";

/**********************************************************************/
/*   Pass2 is the controlling routine of the second pass of the       */
/*        assembler.  It outputs the module name and symbol           */
/*        definitions to the object file and then calls translate     */
/*        to output the object code.  When the end of module          */
/*        signifier is found, debug symbols are output.               */
/**********************************************************************/

pass2 () {

	register int i;

	*extenptr = IFILE;

	if ((ifilebuf = fopen(ifile,"r")) == NULL) {
		faterror(NOOPEN,ifile);
		return (FAILURE);
	}

	if (tmpfile == ifile)
		unlink (ifile);

	if (listopt == TRUE)
		rewind(inbuf);			/* Rewind source file if */
						/*   a listing is desired */

	if ((textbuf = fopen(outfile,"w+")) == NULL) {
		faterror(NOCREATE,srcfile);
		return (FAILURE);
	}

	for (i=0; i<8; i++)
		putw(0,textbuf);

	*extenptr = DATAFILE;		/* Create data object file */
	if ((databuf = fopen(tmpfile,"w+")) == NULL) {
		faterror(NOCREATE,tmpfile);
		return(FAILURE);
	}
	unlink(tmpfile);

	*extenptr = TEXTREL;		/* Create text relocation file */
	if ((reltextbuf = fopen(tmpfile,"w+")) == NULL) {
		faterror(NOCREATE,tmpfile);
		return(FAILURE);
	}
	unlink(tmpfile);

	*extenptr = DATAREL;		/* Create data relocation file */
	if ((reldatabuf = fopen(tmpfile,"w+")) == NULL) {
		faterror(NOCREATE,tmpfile);
		return(FAILURE);
	}
	unlink(tmpfile);

	op1 = &oprand[0];	/* Set up pointers to operands */
	op2 = &oprand[1];
	op3 = &oprand[2];
	op4 = &oprand[3];

	translate();			/* Translate ifile to object file */
}


/************************************************************************/
/*   Terminate closes files and wraps up pass 2.			*/
/************************************************************************/

terminate () {

	listerrors();

	if (numerrors == 1)
		finalmess[9] = ' ';

	if ((numerrors > 0) && listopt)
		printf(finalmess,numerrors);

	if (textbyte == TRUE) {
		putcheck(0,textbuf);
		putrelbyte(reltextbuf);
	}

	putcheck(0,databuf);
	putrelbyte(reldatabuf);

	fclose(inbuf);			/* Close source file */
	fclose(ifilebuf);		/* Close intermediate file */

	concatfiles();			/* Concatenate part of object files */
}


/********************************************************************/
/*   Concatfiles concatenates the object files into the a.out format.	*/
/************************************************************************/

concatfiles () {

	register unsigned words;
	register int i;
	struct exec header;
	register int descrip;

	rewind(databuf);
	rewind(reltextbuf);
	rewind(reldatabuf);

	words = datalc / 2;
	while (words--)
		putw(getw(databuf),textbuf);

	words = textrel / 2;
	while (words--)
		putw(getw(reltextbuf),textbuf);

	words = datarel / 2;
	while (words--)
		putw(getw(reldatabuf),textbuf);


	putsyms();
	rewind(textbuf);

	put2c(A_MAGIC1,textbuf);
	put2c(((textlc+1) >> 1) << 1,textbuf);
	put2c(((datalc+1) >> 1) << 1,textbuf);
	put2c(((bsslc+1) >> 1) << 1,textbuf);
	put2c(symsize,textbuf);

	fclose(textbuf);
	fclose(databuf);
	fclose(reltextbuf);
	fclose(reldatabuf);

	if (numerrors && !forceobject)
		unlink(outfile);
	else if (undef)
		chmod (outfile, 0666);
	else
		chmod (outfile, 0777);
}


/**********************************************************************/
/*   Translate is the main loop of the intermediate file to object    */
/**********************************************************************/

translate () {

	register int messlen;
	register unsigned newlocntr;
	register int siz;

	list = TRUE;
	sc = N_TEXT;
	locntr = 0;
	linenum = 1;
	itag = getbyte();
	if (narrow == FALSE) {
		objcols = 18;
		obj1col = 7;
		obj2col = 13;
	}
	else {
		objcols = 15;
		obj1col = 5;
		obj2col = 10;
	}
	extcnt();

	while (itag != ENDMODULE) {
		switch (itag) {

		case ERROR:
			error2(getbyte());
			itag = getbyte();
			break;

		case ERROR2:
			itag = getbyte();
			siz = getword();
			error2(itag,siz);
			itag = getbyte();
			break;

		case CRET:
			linetransition();
			numcr += getbyte();
			extrasource = FALSE;
			linetransition();
			itag = getbyte();
			break;

		case LOCNT:
			newlocntr = getword();
			if (newlocntr < locntr)
				error2(ADDRESS);
			if (newlocntr == locntr + 1) {
				forcelocntr();
				switch (sc) {
				case N_TEXT:
					putcheck(0,textbuf);
					putrelbyte(reltextbuf);
					break;
				case N_DATA:
					putcheck(0,databuf);
					putrelbyte(reldatabuf);
					break;
				}
				locntr++;
				putlist('\n');
				listcol = 0;
			}
			while (newlocntr > locntr + 1) {
				switch (sc) {
				case N_TEXT:
					put2c(0,textbuf);
					put2c(0,reltextbuf);
					textrel += 2;
					break;
				case N_DATA:
					put2c(0,databuf);
					put2c(0,reldatabuf);
					datarel += 2;
					break;
				}
				locntr += 2;
			}
			if (newlocntr > locntr) {
				switch (sc) {
				case N_TEXT:
					putcheck(0,textbuf);
					putrelbyte(reltextbuf);
					break;
				case N_DATA:
					putcheck(0,databuf);
					putrelbyte(reldatabuf);
					break;
				}
				locntr++;
			}

			itag = getbyte();
			break;

		case RELOCNT:
			rellc = getword();
			itag = getbyte();
			break;

		case FORCELC:
			forcelocntr();
			itag = getbyte();
			break;

		case INST1:
		case INST2:
			instructiontag();
			break;

		case N_DATA:
		case N_TEXT:
		case N_BSS:
			sc = itag;
			locntr = getword();
			itag = getbyte();
			break;

		case LISTON:
			list = TRUE;
			itag = getbyte();
			break;

		case LISTOFF:
			flushline();
			list = FALSE;
			itag = getbyte();
			break;

		case PAGE:
			linetransition();
			putlist('\014');
			putlist('\r');
			listcol = 0;
			itag = getbyte();
			break;

		case ASCII:
			siz = getbyte();
			while (siz-- > 0) {
				putbyte(getbyte());
				locntr++;
			}
			itag = getbyte();
			break;

		case BVAL:
		case WVAL:
		case LVAL:
		case REP:
			datainit();
			break;

		case -1:
			error2(BUG);
			exit();

		default:
			itag = getbyte();
			error2(BUG);
		}
	}
}


/************************************************************************/
/*   Extcnt associates a number with each external.			*/
/************************************************************************/

extcnt () {
	register int extcnt;
	register int id;

	extcnt = 0;

	for (id = firstid; id < nextpos; ) {
		symptr = getsym(id);
		symptr->STYPEPTR = extcnt++;
		if (uflag == TRUE)
			if (symptr->SECTNUM == N_UNDF)
				symptr->SECTNUM = N_EXT;
		id += sizeof (SYMBOL) + symptr->SIZE - 1;
		if (id%2 == 1)
			id++;
	}
}


/************************************************************************/
/*   Putsyms puts out symbols to the output file.			*/
/************************************************************************/

putsyms () {

	register int i;
	register unsigned id;
	register int namsiz;
	register unsigned locval;

	symsize = 0;

	for (id = firstid; id < nextpos; ) {
		symptr = getsym(id);

		locval = symptr->SLOCATION;
		if ((symptr->SECTNUM & ~N_EXT) == N_DATA) {
			locval += textlc;
		}
		else if ((symptr->SECTNUM & ~N_EXT) == N_BSS) {
			locval += datalc;
			locval += textlc;
		}

		namsiz = 0;


		for(i=0; namsiz < 8 && i < symptr->SIZE; i++, namsiz++)
			putcheck(symptr->NAME[i],textbuf);

		for(; namsiz < 8; namsiz++)
			putcheck(0, textbuf);

		symsize += 12;

		put2c(symptr->SECTNUM,textbuf);
		put2c(locval,textbuf);

		id += sizeof (SYMBOL) + symptr->SIZE - 1;
		if (id%2 == 1)
			id++;
	}
}


/**********************************************************************/
/*   Datainit handles the initialization of data.  If it finds a REP  */
/*        tag, it passes it on to the object file, otherwise it       */
/*        evaluates the entry and passes it to the object file.       */
/**********************************************************************/

datainit () {

	register int oprsize;
	OPERANDDESC opr;
	register int repcount;

	repcount = 1;

	if (itag == REP) {
		repcount = getbyte();
		itag = getbyte();
	}

	oprsize = size(itag);
	itag = getbyte();
	getoperand(0);

		if (oprsize == CHARSIZE) {
			if ((oprand[0].DTYPE & ~N_EXT) != N_ABS)
				error2(CONSTERR);
			oprand[0].DTYPE = N_BYTE;
		}

	while (repcount--) {
		putopr(&oprand[0].DTYPE,oprsize);
	}
}


/************************************************************************/
/*   Size determines the size of a data element in bytes		*/
/************************************************************************/

size (tag)
register int tag;

{
	switch (tag) {

	case BVAL:
		return(CHARSIZE);

	case WVAL:
		return (INTSIZE);

	case LVAL:
		return (LONGSIZE);
	}
}


/************************************************************************/
/*   Getword retrieves a word from the intermediate file (IFILE) and	*/
/*	returns it to the calling routine.				*/
/************************************************************************/

getword () {
register unsigned val;


	val = getc(ifilebuf) << 8;
	val += getc(ifilebuf);
	if (p2trace == TRUE)
		printf("\nIFILE WORD = %4x",val);

	return(val);
}


/************************************************************************/
/*   Getbyte returns a single byte value from the IFILE.		*/
/************************************************************************/

getbyte () {
register int val;

	val = getc(ifilebuf) & BYTEMASK;
	if (p2trace == TRUE)
		printf("\nIFILE BYTE = %2x",val);

	return(val);
}


/************************************************************************/
/*   Putbyte puts a byte to the output file and the listing file	*/
/************************************************************************/

putbyte (byteval)

register unsigned byteval;
{

	switch (sc) {
	case N_TEXT:
		putcheck(byteval,textbuf);
		putrelbyte(reltextbuf);
		break;
	case N_DATA:
		putcheck(byteval,databuf);
		putrelbyte(reldatabuf);
		break;
	}
	if (locntr%2 == 1)
		if ((listcol == obj1col) || (listcol == obj2col)) {
			putlist(' ');
			putlist(' ');
		}
	listbyte(byteval);
	if ((locntr%2 == 1) && (listcol == obj1col + 4)) {
		putlist(' ');
		if (narrow == FALSE)
			putlist(' ');
	}
}


/************************************************************************/
/*   Putrelbyte puts an absolute relocation word into the relocation    */
/*      file every two bytes.  It needs to be called every time a lone  */
/*      (therefore absolute) byte is output .				*/
/************************************************************************/

putrelbyte (buf)

FILE *buf;
{

	static saveflag = 0;

	if (saveflag++) {
		put2c(0,buf);
		if (buf == reltextbuf)
			textrel += 2;
		else
			datarel += 2;
		saveflag = 0;
	}
}


/************************************************************************/
/*   Putword puts a word to the output file.  If it is not an absolute	*/
/*	word, the relocation information is also put into the output	*/
/*	file.  The list file is updated annually.			*/
/************************************************************************/

putword (wordtype, word)

register int wordtype;
register unsigned word;

{
	register unsigned relval;
	register unsigned oldword;

	oldword = word;

	switch (wordtype & ~N_EXT) {
	case N_TEXT:
		relval = RTEXT;
		break;
	case N_DATA:
		relval = RDATA;
		word += textlc;
		break;
	case N_BSS:
		relval = RBSS;
		word += textlc;
		word += datalc;
		break;
	case N_ABS:
		relval = 0;
		break;
	default:
		relval = wordtype;
		break;
	}

	switch (sc) {
	case N_TEXT:
		listhex(oldword);
		if ((relval >= 2) && (relval <= 6))
			word += rellc;
		put2c(word,textbuf);

		put2c(relval,reltextbuf);
		textrel += 2;
		break;

	case N_DATA:
		listhex(oldword);
		if ((relval >= 2) && (relval <= 6))
			word += rellc;
		put2c(word,databuf);

		put2c(relval,reldatabuf);
		datarel += 2;
		break;
	}
	putexten(wordtype);
	if ((listcol < obj2col) && (narrow == FALSE))
		putlist(' ');
}


/************************************************************************/
/*   Put2c puts out a word in the Z8000 format, with bytes swapped	*/
/*	when compared with the PDP-11.					*/
/************************************************************************/

put2c (val, buffer)

register char *buffer;
register unsigned val;

{
	putcheck(val>>8,buffer);
	putcheck(val,buffer);
}


/**********************************************************************/
/*   Putexten puts out the proper extension character for this word.  */
/**********************************************************************/

putexten (wordtype)
register int wordtype;

{
	switch (wordtype) {

	case N_ABS:
	case N_ABS + N_EXT:
		putlist(' ');
		break;

	case N_TEXT:
	case N_TEXT + N_EXT:
		putlist('t');
		break;

	case N_DATA:
	case N_DATA + N_EXT:
		putlist('d');
		break;

	case N_BSS:
	case N_BSS + N_EXT:
		putlist('b');
		break;

	case N_UNDF:
		undef++;
		putlist('?');
		error2(UNDEFSYM);
		break;

	default:
		undef++;
		putlist('x');
		break;
	}
}


/**********************************************************************/
/*   Linetransition puts out a source line, its errors, and all       */
/*        following source lines until either code is generated       */
/*        or a location counter needs to be listed.                   */
/**********************************************************************/

linetransition () {

	if ((listcol != 0) && (extrasource == TRUE)) {
		putlist('\n');
		listcol = 0;
		listerrors();
	}

	while (numcr > 0)
		flushline();
}


/************************************************************************/
/*  Flushline flushes out the next source line.				*/
/************************************************************************/

flushline () {

	if (numcr < 0)
		return;
	if ((listcol == 0) && (margin == TRUE))
			putlist('\t');
	listblanks(objcols - listcol);
	listsource();
	listerrors();
	numcr--;
}


/**********************************************************************/
/*   Listblanks puts the appropriate number of blanks in the source   */
/*        listing (this is to space past the object columns).         */
/**********************************************************************/

listblanks (bcount)

register int bcount;

{
	while (bcount-- > 0)
		putlist(' ');
}


/**********************************************************************/
/*   Listsource reads the source input while writing it to the        */
/*        list file until a newline character is found.               */
/**********************************************************************/

listsource () {

	char ch;

	if ((listopt == TRUE) && (list == TRUE)) {
		if (narrow == FALSE) {
			if (linenum > 999)
				putlist(' ');
			listdec(linenum);
			}
		putlist('\t');
	}

	if (listopt == TRUE) {
		do {
			ch = getc(inbuf);
			if (list == TRUE)
				putlist(ch);
		}
		while ((ch != '\n') && (ch > 0));
	}

	if (numcr <= 0)
		extrasource = TRUE;
	else
		extrasource = FALSE;

	linenum++;
	listcol = 0;
}


/**********************************************************************/
/*   Listerrors prints out the error numbers encountered on the       */
/*        last line of source.  It assumes that error2 has taken      */
/*        care of duplicates.                                         */
/**********************************************************************/

listerrors () {

	register int i;
	register int j;

	if (errorcount == 0)
		return;

	if (margin == TRUE)
		putlist('\t');

	printdev2("as:\t%5d:\t",linenum-1);

	for (i=0; i<errorcount; i++) {
		if (i != 0) printdev2("\t\t");
		printdev2(errmsg[errorbuf[i]],symbuf[i]);
	}

	if ((listopt == TRUE) && (list == TRUE))
		for (i = 0; i < errorcount; i++) {
			if (i>0 && margin==TRUE)
				putlist('\t');
			for (j = leftmargin; j < objcols - 10; j++)
				putlist('*');
			putlist(' ');
			putlist(' ');

			printf("\t");
			printf(errmsg[errorbuf[i]],symbuf[i]);
		}

	listcol = 0;
	errorcount = 0;
}


/**********************************************************************/
/*   Forcelocntr puts out the location counter if the current column  */
/*        is the leftmost one.                                        */
/**********************************************************************/

forcelocntr () {


	linetransition();

	if (listcol == 0) {
		if (margin == TRUE)
			putlist('\t');
		if (narrow == FALSE)
			switch (sc) {
			case N_TEXT:
				putlist(' ');
				break;
			case N_DATA:
				putlist('d');
				break;
			case N_BSS:
				putlist('b');
				break;
			}

		listhex(locntr);
		putlist(' ');
		if (narrow == FALSE)
			putlist(' ');
	}
}


/************************************************************************/
/*   Listval puts out a hex value followed by an extension character.	*/
/************************************************************************/

listval (val, exten)

register unsigned val;
register int exten;
{

	listhex(val);
	putlist(exten);
}


/**********************************************************************/
/*   Putlist puts out a single character if the listing option        */
/*        is true and the list option is turned.                      */
/**********************************************************************/

putlist (ch)

char ch;

{
	if ((listopt == TRUE) && (list == TRUE)) {
		putchar(ch);
	}

	if ((ch != '\n') && (ch != '\t'))
		listcol++;
}


/**********************************************************************/
/*   Listdec lists a decimal number, takes four columns, and          */
/*        blank fills.                                                */
/**********************************************************************/

listdec (decnum)

register int decnum;

{
	if ((listopt == TRUE) && (list == TRUE))
		printf("%4d", decnum);

	listcol += 4;
}


/**********************************************************************/
/*   Listhex lists a hex value, zero filled.                          */
/**********************************************************************/

listhex (hexvalue)

register int hexvalue;

{
	register int i;

	if (listcol > objcols - 4) {
		flushline();
		forcelocntr();
	}

	if ((listopt == TRUE) && (list == TRUE))
		for (i=0; i<4; i++)
			printf("%1x",(hexvalue >> (12-4*i)) & 0X0F);

	listcol += 4;
}


/************************************************************************/
/*   Listbyte lists a byte value in the listing file.			*/
/************************************************************************/

listbyte (bytevalue)

register unsigned bytevalue;
{

	register int i;

	if (listcol > objcols - 2) {
		flushline();
		forcelocntr();
	}

	if ((listopt == TRUE) && (list == TRUE))
		for (i=0; i<2; i++)
			printf("%01x",(bytevalue >> (4-4*i)) & 0X0F);

	listcol += 2;
}


/**********************************************************************/
/*   Error2 puts error numbers in the error buffer, making sure       */
/*        that numbers aren't repeated.                               */
/**********************************************************************/

error2 (errornum, symbol)

register int errornum;
register unsigned symbol;

{
	register int i;

	if (p2trace == TRUE)
		printf("\n*****ERROR %d",errornum);


	if (errorcount > MAXERRORCOUNT)
		return;

	for (i = 0; i < errorcount; i++) {
		if (errorbuf[i] == errornum)
			return;
	}

	symbuf[errorcount] = symbol;
	errorbuf[errorcount++] = errornum;
	numerrors++;
}
