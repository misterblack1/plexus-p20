#include <stdio.h>

/**********************************************************************/
/*   Module inst parses an entire assembly language intruction        */
/*        and outputs it to the intermediate file.  Operand           */
/*        combinations are checked for validity for the corresponding */
/*        instructions.  If new types of operands are being added,    */
/*        this module (along with the tables) needs to be modified.   */
/**********************************************************************/


#include <a.out.h>
#include "class.decs"
#include "common.decs"
#include "error.decs"
#include "ifile.decs"
#include "mask.decs"
#include "scanner.decs"
#include "symbol.decs"
#include "symtab.decs"
#include "token.decs"
#include "types.decs"

#include "extern.decs"
#include "extern1.decs"
 

/**********************************************************************/
/*   Labelstatement parses labels and statements, otherwise it        */
/*        reports an error and returns.                               */
/**********************************************************************/

labelstatement () {

	register unsigned labelid;
	register int loctok;
	register int retval;
	long val;

	switch (tok) {

	case DOTV:
	case DOTDOTV:
		loctok = tok;

		iputeols();
		scan();
		if (tok != EQV)
			error1(ILLSTAT);
		else {
			scan();		/* Scan past equal sign */
			retval = constexpr(&val);
			if (loctok == DOTDOTV) {
				if (retval != N_ABS)
					error1(ILLSTAT);
				rellc = LOWORD(val);
				iputbyte(RELOCNT);
				iputword(rellc);
				break;
			}
			if (retval != sc) {
				error1(ILLCEXPR);
				break;
			}
			if (lc <= LOWORD(val))
				lc = LOWORD(val);
			else
				error1(DOTDECREMENT);
		}
		break;

	case VAR:
		iputeols();
		labelid = curid;

		scan();			/* Scan past symbol */
		if (tok == EQV) {
			checkdef(labelid);
			class['~'] = OPR_CLS;
			scan();
			retval = constexpr(&val);
			class['~'] = ID_CLS;
			if ((retval & ~N_EXT) == N_UNDF)
				error1(ILLCEXPR);
			else {
				symptr = getsym(labelid);
				symptr->SECTNUM |= retval & ~N_EXT;
				symptr->STYPEPTR = HIWORD(val);
				symptr->SLOCATION = LOWORD(val);
			}
		}
		else if (tok == COLONV) {
			forcelc();
			scan();
			checkdef(labelid);
			symptr = getsym(labelid);
			symptr->SECTNUM |= sc;
			symptr->SLOCATION = lc;
			symptr->STYPEPTR = N_UNDF;
		}
		else if (zcc == FALSE)
			error1(ILLSTAT);
		else {
			forcelc();
			iputtag(WVAL);
			iputwval(EXREF,labelid);
			iputlc(2);
		}
		break;

	case CONSTVAR:
	case NUM:
	case STR:
		if (zcc == TRUE) {
			forcelc();
			iputtag(WVAL);
			iputwval(LDWORD,slocation);
			iputlc(2);
		}
		scan();
		break;

	case OPCODE :
		asminstruction();
		break;

	case CONTROL :
		controline();
		break;
	
	case XOPCODE:
		xopcode();
		break;

	default:
		return(FAILURE);
	}

	return(SUCCESS);

} 


/************************************************************************/
/*   Checkdef checks to see if the current symbol has already been	*/
/*	defined.  If so, the appropriate error message is generated.	*/
/************************************************************************/

checkdef (id)

register unsigned id;
{
	symptr = getsym(id);
	if ((symptr->SECTNUM & ~N_EXT) != N_UNDF)
		symerror(MULTSYM,curid);
}


/**********************************************************************/
/*   Asminstruction collects the operands and calls the routine       */
/*        to output the instruction (or report an error) to the       */
/*        intermediate file.                                          */
/**********************************************************************/

asminstruction () {

	register unsigned instid;
	register int opcount;

	class['~'] = OPR_CLS;
	iputeols();
	opbufptr = &opbuffer[0];
	oprptr = &oprbuf[0];
	instid = curid;
	opcount = section;
	scan();

	while (--opcount >= 0) {
		if (oper2() == FAILURE) break;
		if ((opcount == 0) || (tok != COMMAV)) break;
		scan();			/* Scan past comma */
	}

	outinst(instid);
	class['~'] = ID_CLS;
}


/***********************************************************************/  
/*  Operand2 parses a single operand, placing the result in the buffer */
/*   opbuffer. The type of operand is placed in the buffer oprbuf.     */
/***********************************************************************/

oper2 () {

	register unsigned mode;
	register unsigned result;

	result = SUCCESS;
	mode = tok;

	switch (tok) {

	case DOLLARV:
		scan();			/* Scan past '$' */
		if ((result = oprexpr()) != FAILURE)
		newop(IMMED);
		break;

	case STARV:
		scan();			/* Scan past '*' */
		if (tok == REG)
			newop(INDREG);
		else {
			error1(ILLOPER);
			break;
		}
		outbyteloc();
		scan();			/* Scan past register */
		break;

	case CNDCODE:
	case CTLREG:
	case FLAGREG:
	case INTRPT:
	case REGQUAD:
	case REGHILO:
	case FREG:
		newop(tok);
		outbyteloc();
		scan();			/* Scan past register */
		break;
	
	case REG:
	case REGPAIR:
		outbyteloc();
		scan();			/* Scan past register */
		
		if (tok == LPARENV) {
			 scan();
			 if (tok == REG) {
				outbyteloc();
				scan();	/* Scan past register */
				newop(BINDEX);
			 }
			 else if (tok == DOLLARV) {
				scan();
				if ((result = oprexpr()) != FAILURE)
					 newop(BADDR);
				}
				else {
					error1(ILLOPER);
					return(FAILURE);
				}
				if (tok != RPARENV) {
					error1(NORPAREN);
					return(FAILURE);
				}
				else scan();
		}
		else  newop(mode);
		
		break;

	case EOFV:
	case NEWLINEV:
	case SEMIV:
		result = FAILURE;
		break;

	default:

		if((result = oprexpr()) == FAILURE) break;

		if (tok == LPARENV) {
			scan();
			if (tok != REG) {
				error1(ILLINST);
				return(FAILURE);
			}
			symptr = getsym(curid);
			if (symptr->SLOCATION == 0) {
				error1(ILLINDEX);
				return(FAILURE);
			}  /* can't index through r0 */


			outbyteloc();
			newop(INDEXED);
			scan();

			if (tok != RPARENV) {
				error1(NORPAREN);
				return(FAILURE);
			}
			scan();
		}
		else
			newop(IDENT);
	}

	return(result);
}


/**********************************************************************/
/*   Outinst locates the proper instructions (or an error if appro-   */
/*        priate), word aligns the instructions, determines the size  */
/*        of the instruction and outputs the instruction and          */
/*        operators, and updates the location counter.                */
/**********************************************************************/

outinst (symid)

register int symid;

{
	register unsigned instr;		/* Current instruction number */
	register int pos;		/* Format of current instruction */
	register int siz;		/* Size of current instruction in bytes */
	
	instr = findinst(symid);

	even();

	forcelc();

	if (instr == BADINST)  {
		error1(ILLINST);
		iputlc(4);
		iputbval(INST2, BADINST);
		return;
	}

	pos = opcodetable[instr].IFORMAT;

	siz = formattable[pos].BYTECOUNT;
	
	iputlc(siz);

	if (instr <= 255)
		iputbval(INST1,instr);
	else
		iputbval(INST2,instr - 255);

	if (p1trace == TRUE) {
		printf("\n***********************************Instruction %d\n",
			instr);
	}

	iputopr();
}


/**********************************************************************/
/*   Findinst searches all the operand combinations for this          */ 
/*        particular instruction for a match with the operands in     */
/*        oprbuf.  Either FAILURE or the instruction's number is      */
/*        returned.                                                   */
/**********************************************************************/

findinst (symid)

register int symid;

{
	register int i;			/* Local counter variable */
	register int pos;		/* Current instruction opcode offset */
	register int siz;		/* Number of entries for current instruction */
	register unsigned inst;

	symptr = getsym(symid);

	while (oprptr < endoprbuf)
		*oprptr++ = NILWORD;

	pos = symptr->SLOCATION;
	siz = symptr->STYPEPTR;

	for (i = 0; i < siz; i++) {
		inst = pos + i;
		if (chkoperands(opcodetable[inst].OPERAND) == SUCCESS)
			return(inst);
	}
	return (BADINST);

}

/**********************************************************************/
/*   Chkoperands looks through the operandtable trying to match       */
/*        operands. As soon as it finds a matching sequence, the      */
/*        offset is returned to the calling routine.                  */
/**********************************************************************/

chkoperands (opr)

register int opr;
 
{
	register int bufpos;
	register int i;
	register int j;
	char oprclass;
	register int pos;

	opr &= BYTEMASK;
	bufpos = 0;

	for (pos = 0; pos < OPTABWIDTH;) {
		oprclass = operandtable[opr][pos];

		if (oprclass == CON) {
			opr++;
			pos = 0;
			continue;
		}

		for (i = 0; i < MAXCLASS; i++) {

			if (p1trace == TRUE)
			printf("\nOPRBUF [%d] = %o, CLASSTABLE [%d] [%d] = %o",
				bufpos, oprbuf[bufpos], oprclass, i,
				classtable[oprclass][i]);
			if (oprbuf[bufpos] == classtable[oprclass][i]) {
				pos++;
				bufpos++;
				break;
			}
		}
		
		if (i >= MAXCLASS) return (FAILURE);
	}
	if (oprbuf[bufpos] != NILWORD)
		return (FAILURE);

	return (SUCCESS);
}


/************************************************************************/
/*   Getinitval parses a initial value expression and places it in	*/
/*	the intermediate file.						*/
/************************************************************************/

getinitval (type)
register int type;
{

	register unsigned result;

	opbufptr = &opbuffer[0];
	oprptr = &oprbuf[0];

	if ((result = oprexpr()) == FAILURE)
		error1(ILLCEXPR);
	else {
		iputtag(type);
		iputopr();
	}
	iputeols();
}


/************************************************************************/
/*   Updatelc updates the current location counter to correspond to the */
/*	new section.							*/
/************************************************************************/

updatelc (newsc)

register int newsc;

{

	switch (sc) {

	case N_BSS:
		bsslc = lc;
		break;

	case N_TEXT:
		textlc = lc;
		break;

	case N_DATA:
		datalc = lc;
		break;
	}

	switch (newsc) {

	case N_BSS:
		lc = bsslc;
		break;

	case N_TEXT:
		lc = textlc;
		break;

	case N_DATA:
		lc = datalc;
		break;
	}
}
