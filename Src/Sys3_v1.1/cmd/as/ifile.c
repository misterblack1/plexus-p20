#
/*        pass1 and the intermediate file (IFILE).  Some         */
/*        optimization is and still can be done, including       */
/*        not outputting section counters and location           */
/*        counters unless absolutely necessary.  Currently       */
/*        pass2 takes care of compaction, but reduction in the   */
/*        size of the IFILE might improve speed.                 */
/*****************************************************************/


#include <stdio.h>
#include "common.decs"
#include "errmsg.decs"
#include "error.decs"
#include "ifile.decs"
#include "mask.decs"
#include "types.decs"

#include "extern.decs"
#include "extern1.decs"


/*****************************************************************/
/*   Newop places the operand type given as an argument into the */
/*        oprbuf buffer.                                         */
/*****************************************************************/

newop (opr)

register int opr;

{
	lastopr = oprptr;
	*oprptr++ = opr;
}


/*****************************************************************/
/*   Deloprbyte deletes the EXPR tag byte from the operand       */
/*        buffer (opbuffer).  It is assumed that beginexpr       */
/*        points to that byte.                                   */
/*****************************************************************/

deloprbyte () {

	register char *ptr;

	for (ptr = beginexpr; ptr < opbufptr - 1; ptr++)
		*ptr = *(ptr + 1);

	opbufptr--;
}


/*****************************************************************/
/*   Removeop removes the last operand type from the oprbuf      */
/*        buffer.  Lastopbufptr is no longer the previous        */
/*        operand following execution of this function.          */
/*****************************************************************/

removeop () {

	oprptr--;
	opbufptr = beginexpr;
}


/*****************************************************************/
/*   Opputword outputs a word to the opbuffer as two bytes.      */
/*****************************************************************/

opputword (val)

register int val;

{
	opputbyte(val >> 8);
	opputbyte(val);
}


/*****************************************************************/
/*   Opputbyte checks for buffer overflow and outputs a byte     */
/*        to the opbuffer.                                       */
/*****************************************************************/

opputbyte (val)

register int val;

{
	if (opbufptr > endopbuf)
		error1(EXPCOMP);
	else
		*opbufptr++ = val;
}


/**********************************************************************/
/*   Eolerror puts the eols followed by an error message into         */
/*        the IFILE.                                                  */
/**********************************************************************/

eolerror (errno)

register int errno;

{
	iputeols();
	error1(errno);
}


/************************************************************************/
/*   Symerror1 outputs an error number and symbol to the IFILE.		*/
/************************************************************************/

symerror1 (errno, symbol)

register int errno;
register unsigned symbol;
{
	iputbval(ERROR2,errno);
	iputword(symbol);
}



/*****************************************************************/
/*   Error outputs an error tag and error number to the IFILE.   */
/*        If the pass1 trace is on, it also prints the error1.    */
/*****************************************************************/

error1 (errno)

register int errno;

{
	if (noerror == TRUE)
		return;

	iputbval(ERROR,errno);

	if (verbose != FALSE)
		printf("\n*****ERROR %d*****\n",errno);
}


/*****************************************************************/
/*   Iputeols adds eols to the global variable linenum, so that  */
/*        the synchronization can take place with the listing    */
/*        part of pass2.                                         */
/*****************************************************************/

iputeols () {

	eolcnt += eols;
	eols = 0;
}


/*****************************************************************/
/*   Forcelc outputs a force location counter tag to insure that */
/*        a location counter occurs at this point in the         */
/*        listing.                                               */
/*****************************************************************/

forcelc () {
	iputlc(0);
	iputtag(FORCELC);
}


/*****************************************************************/
/*   Iputopr puts the current operand operand list stored        */
/*        temporarily in opbuffer into the IFILE.                */
/*****************************************************************/

iputopr () {

	register char *ptr;

	for (ptr = &opbuffer[0]; ptr < opbufptr; ptr++)
		iputbyte(*ptr);

	opbufptr = &opbuffer[0];
}


/****************************************************************/
/*   Even aligns the location counter on an even boundary.	*/
/****************************************************************/

even () {

	if ((lc %2) != 0)
		lc++;
}


/*****************************************************************/
/*   Iputlc puts out the current location counter if it has      */
/*        changed or if it is required at this point.            */
/*****************************************************************/

iputlc (siz)

register unsigned siz;

{
	if (nextloc != lc || locflag == FALSE)
		iputwval(LOCNT,lc);

	if ((lc + siz) < lc)
		error1(ADDRESS);

	nextloc = (lc += siz);

	locflag = TRUE;
}


/*****************************************************************/
/*   Iputlval puts out an entry consisting of a tag and a        */
/*        long word to the IFILE.                                */
/*****************************************************************/

iputlval (tag, val)

long val;
register int tag;

{
	iputtag(tag);
	
	iputword(val>>16);
	iputword(val);
}


/*****************************************************************/
/*   Iputwval puts a tag and word value out to the IFILE.        */
/*****************************************************************/

iputwval (tag, val)

register int tag;
register int val;

{
	iputtag(tag);
	iputword(val);
}


/**********************************************************************/
/*   Iputbval puts a tag and byte value out to the IFILE.             */
/**********************************************************************/

iputbval (tag, val)

register int tag;
register int val;

{
	iputtag(tag);
	iputbyte(val);
}


/*****************************************************************/
/*   Iputtag checks to see if eols need to be output, then       */
/*        outputs the tag passed as an argument to the IFILE.    */
/*****************************************************************/

iputtag (tag)

register int tag;

{
	linenum += eolcnt;

	while (eolcnt > 255) {
		iputbyte(CRET);
		iputbyte(255);
		eolcnt -= 255;
	}
	if (eolcnt != 0) {
		iputbyte(CRET);
		iputbyte(eolcnt);
		eolcnt = 0;
	}

	iputbyte(tag);
}


/************************************************************************/
/*   Iputword puts a word into the intermediate file.			*/
/************************************************************************/

iputword (val)

register int val;

{
	iputbyte(val>>8);
	iputbyte(val);
}


/************************************************************************/
/*   Iputbyte puts a single byte into the intermediate file.          */
/**********************************************************************/

iputbyte (val)

register int val;

{
	putcheck(val,ifilebuf);
}


/************************************************************************/
/*   Outbyteloc outputs the value of the slocation field of the current	*/
/*	symbol into the opbuffer.  Only a single byte is output.	*/
/************************************************************************/

outbyteloc () {

	symptr = getsym(curid);
	opputbyte(LDBYTE);
	opputbyte(symptr->SLOCATION);
}
