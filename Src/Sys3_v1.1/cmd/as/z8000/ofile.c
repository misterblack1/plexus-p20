#
/*        intermediate file.  it retrieves the instruction, evaluates */
/*        the operands, and outputs object code according to          */
/*        information found in tables.  If an instruction doesn't     */
/*        end on a word boundary, a bug error message is generated    */
/*        and should be checked out.                                  */
/**********************************************************************/


#include <a.out.h>
#include <stdio.h>
#include "common.h"
#include "error.h"
#include "ifile.h"
#include "opcode.h"
#include "operand.h"
#include "types.h"

#include "extern2.h"
#include "extern.h"


/**********************************************************************/
/*   Instructiontag retrieves the operands for the current            */
/*        instruction, and calls assembleinstr to construct the       */
/*        binary.                                                     */
/**********************************************************************/

instructiontag () {

	register int  col;
	register int  instruction;
	register int  newlocntr;
	register int  opcounter;
	register int  operandentry;
	register int  opnum;
	register int  optype;

	if (itag == INST1)
		instruction = 0;
	else
		instruction = 255;

	instruction += getbyte();

	itag = getbyte();

	operandentry = opcodetable[instruction].OPERAND & BYTEMASK;

	if (instruction == 510) {
		putword(N_ABS,BADINST);
		putword(N_ABS,BADINST);
		locntr += 4;
		return;
	}

		newlocntr = locntr +
			formattable[opcodetable[instruction].IFORMAT].BYTECOUNT;

	for (opnum = 0, opcounter = 0; opcounter < OPTABWIDTH; opnum++,
			opcounter++) {
		optype = operandtable[operandentry][opcounter];

		switch (optype) {

		case NILOP:
			opcounter = OPTABWIDTH;
			continue;

		case CON:
			operandentry++;
			opcounter = -1;
			opnum--;
			continue;

		case X:
		case BA:
		case BX:
			getoperand(opnum++);
			getoperand(opnum);
			break;

		case RA:
		case RABYTE:
		case RANEG:
			getoperand(opnum);

			if ((oprand[opnum].DTYPE & ~N_EXT) != sc) {
				error2(ILLCEXPR);
				break;
			}

			oprand[opnum].DTYPE = N_ABS;
			oprand[opnum].DOPERAND -= newlocntr;

			switch (optype) {
			
			case RA:
				oprand[opnum].DOPERAND >>= 1;
				break;

			case RANEG:
				oprand[opnum].DOPERAND >>= 1;
				oprand[opnum].DOPERAND =
					-oprand[opnum].DOPERAND;
			}
			break;

		case FLAG:
			getoperand(opnum);
			LOWORD(oprand[opnum].DOPERAND) >>= 4;

			if (LOWORD(oprand[opnum].DOPERAND) == 0)
				error2(ILLOPER);
			break;

		case CC:
			getoperand(opnum);
			if(LOWORD(oprand[opnum].DOPERAND) % 16 == 0)
				error2(ILLOPER);
			LOWORD(oprand[opnum].DOPERAND) &= 0X0F;
			break;

		default:
			getoperand(opnum);

		}
	}
	assembleinstr(&opcodetable[instruction].OPECODE);
	locntr = newlocntr;
}


/**********************************************************************/
/*   Assembleinstr uses the formattable to assemble an instruction    */
/*        and puts it in the output and object files.  Bits, nibbles, */
/*        bytes, and words are collected, with code being generated   */
/*        whenever a 16 bit boundary is reached.                      */
/**********************************************************************/

assembleinstr (opcodtabptr)
OPCODENTRY *opcodtabptr;

{

	register int col;
	register int formatentry;
	register unsigned opcode;
	register unsigned opcodeword;

	opcodeword = 0;
	opcode = opcodtabptr->OPECODE;
	formatentry = opcodtabptr->IFORMAT & BYTEMASK;

	for (col = 0; col < MAXFORMATSIZE; col++) {

		if (p2trace == TRUE)
			printf("\nFORMAT -- %d",formattable[formatentry].FIELDS[col]);
		switch (formattable[formatentry].FIELDS[col]) {

		case NILWORD:
			col = MAXFORMATSIZE;
			continue;

		case CON:
			formatentry += 1;
			col = -1;
			continue;

		case OPCW:
			collectopcode(opcode,16);
			break;

		case OPCB1:
			collectopcode(opcode >> 8,8);
			break;

		case OPCN1:
			collectopcode(opcode >> 12, 4);
			break;

		case OPCN3:
			collectopcode(opcode >> 4, 4);
			break;

		case OPCN4:
			collectopcode(opcode, 4);
			break;

		case OPCBT3:
			collectopcode(opcode >> 3, 1);
			break;

		case OPCBT7:
			collectopcode(opcode >> 7, 1);
			break;

		case OPCBTS45:
			collectopcode(opcode >> 4, 2);
			break;
 
		case OPCBTS67:
			collectopcode(opcode >> 6, 2);
			break;

		case OPC7BTS:
			collectopcode(opcode >> 9, 7);
			break;

		case OP1N:
			opcollectopcode(op1, 4);
			break;

		case OP2N:
			opcollectopcode(op2, 4);
			break;

		case OP3N:
			opcollectopcode(op3, 4);
			break;

		case OP4N:
			opcollectopcode(op4, 4);
			break;

		case OP2NM1:
			op2->DOPERAND--;
			opcollectopcode(op2, 4);
			break;

		case OP3NM1:
			op3->DOPERAND--;
			opcollectopcode(op3, 4);
			break;

		case OP4NM1:
			op4->DOPERAND--;
			opcollectopcode(op4, 4);
			break;

		case N0:
			collectopcode(0, 4);
			break;

		case N1:
			collectopcode(0X0F, 4);
			break;

		case N0E:
			collectopcode(0X0E, 4);
			break;

		case B0:
			collectopcode(0, 8);
			break;

		case B1:
			collectopcode(0XFF, 8);
			break;

		case OP1B:
			opcollectopcode(op1, 8);
			break;

		case OP1SB:
			rcollectopcode(op1, 8);
			break;

		case OP2B:
			opcollectopcode(op2, 8);
			break;

		case OP2SB:
			rcollectopcode(op2, 8);
			break;

		case OP3B:
			opcollectopcode(op3, 8);
			break;

		case OP1W:
		case OP1AW:
			opcollectopcode(op1, 16);
			break;

		case OP2W:
		case OP2AW:
			opcollectopcode(op2, 16);
			break;

		case OP2W2:
			op2->DOPERAND = -op2->DOPERAND;
			opcollectopcode(op2, 16);
			break;

		case OP3W:
			opcollectopcode(op3, 16);
			break;

		case OP2L:
			opcollectopcode(op2, 32);
			break;

		case OP3L:
			opcollectopcode(op3, 32);
			break;

		case OP1SC:
			rcollectopcode(op1, 12);
			break;

		case OP12BTS:
			opcollectopcode(op1, 2);
			break;

		case OP13BTS:
			opcollectopcode(op1, 3);
			break;

		case OP19BTS:
			opcollectopcode(op1, 9);
			break;

		case OP23BTS:
			opcollectopcode(op2, 3);
			break;

		case OP24BTS:
			LOWORD(op2->DOPERAND) &= 0X000F;
			opcollectopcode(op2, 16);
			break;

		case OP25BTS:
			LOWORD(op2->DOPERAND) &= 0X001F;
			opcollectopcode(op2, 16);
			break;

		case OP26BTS:
			LOWORD(op2->DOPERAND) &= 0X003F;
			opcollectopcode(op2, 16);
			break;

		case OP23BTS2:
			op2->DOPERAND = -op2->DOPERAND;
			opcollectopcode(op2, 3);
			break;

		case OP24BTS2:
			op2->DOPERAND &= 0X000F;
			op2->DOPERAND = -op2->DOPERAND;
			opcollectopcode(op2, 16);
			break;


		case OP25BTS2:
			op2->DOPERAND &= 0X001F;
			op2->DOPERAND = -op2->DOPERAND;
			opcollectopcode(op2, 16);
			break;

		case OP26BTS2:
			op2->DOPERAND &= 0X003F;
			op2->DOPERAND = -op2->DOPERAND;
			opcollectopcode(op2, 16);
			break;

		case OP27BTS:
			bcollectopcode(op2, 7);
			break;

		case OP215BTS:
			opcollectopcode(op2, 15);
			break;

		case OP2BT1:
			op2->DOPERAND -= 1;
			opcollectopcode(op2, 1);
			break;

		case BT0:
			collectopcode(0, 1);
			break;

		case BT1:
			collectopcode(1, 1);
			break;

		case OR4:
			op3->DOPERAND |= op4->DOPERAND;

		case OR3:
			op2->DOPERAND |= op3->DOPERAND;

		case OR2:
			op1->DOPERAND |= op2->DOPERAND;
			opcollectopcode(op2, 4);
			break;

		case AND22BTS:
			op1->DOPERAND &= op2->DOPERAND;
			opcollectopcode(op1, 2);
			break;

		case WVALUE1:
			collectopcode(1,16);
			break;
		case OP1FD:
			collectopcode(fcheck(op1,4), 1);
			break;
		case OP1FF:
			collectopcode(fcheck(op1,2), 1);
			break;
		case OP1FI:
			collectopcode(fcheck(op1,1), 1);
			break;
		case OP2FD:
			collectopcode(fcheck(op2,4), 1);
			break;
		case OP2FF:
			collectopcode(fcheck(op2,2), 1);
			break;
		case OP2FI:
			collectopcode(fcheck(op2,1), 1);
			break;
		default:
			error2(BUG);

		}
	}
}


/**********************************************************************/
/*   Opcollectopcode puts out an operand.  By definition, it must     */
/*        be a word or long word.  If any stray bits have not been    */
/*        processed, then an internal error condition exists.         */
/**********************************************************************/

opcollectopcode (opr, bitcount)

register OPERANDDESC *opr;
register int  bitcount;

{
	if (bitcount < 16) {
/* taken out to keep from causing confusing messages from compiler
		if ((opr->DTYPE & ~N_EXT) != N_ABS)
			error2(ILLOPER);
*/
		if (bitcount < 8)
			xcollectopcode(opr,bitcount);
		else
			collectopcode(LOWORD(opr->DOPERAND),bitcount);
	}

	else {
		if (bitsused != 0) {
			error2(BUG);
			bitsused = 0;
		}
		putopr(opr, bitcount);
	}
}


/**********************************************************************/
/*   Putopr puts out an operand into the object file and list file.   */
/*        It takes care of putting out the appropriate tags for       */
/*        ABSOLUTE, REL, and EXT.                                          */
/**********************************************************************/

putopr (opr, oprsize)

register OPERANDDESC *opr;
register int  oprsize;

{

	switch (opr->DTYPE) {

	case N_UNDF:
		putword(N_UNDF,0);
		break;

	case N_BYTE:
		putbyte(LOWORD(opr->DOPERAND));
		break;

	case N_ABS + N_EXT:
	case N_ABS:
		if (oprsize < INTSIZE) {
			collectopcode(LOWORD(opr->DOPERAND),oprsize);
			break;
		}
		else
			if (oprsize == LONGSIZE) {
				putword(N_ABS,HIWORD(opr->DOPERAND));
				locntr += INTSIZE/8;
				oprsize -= INTSIZE;
			}

		putword(N_ABS,LOWORD(opr->DOPERAND));
		break;

	case N_TEXT:
	case N_TEXT + N_EXT:
	case N_DATA:
	case N_DATA + N_EXT:
	case N_BSS:
	case N_BSS + N_EXT:
		if (oprsize < INTSIZE) {
			collectopcode(LOWORD(opr->DOPERAND),oprsize);
			break;
		}
		else
			if (oprsize == LONGSIZE) {
				putword(N_ABS,0);
				locntr += INTSIZE/8;
				oprsize -= INTSIZE;
			}

		putword(opr->DTYPE,LOWORD(opr->DOPERAND));
		break;

	case N_EXT:
		if (oprsize == LONGSIZE) {
			putword(N_ABS,0);
			locntr += INTSIZE/8;
			oprsize -= INTSIZE;
		}
		putword((opr->DISPLAC<<4) + 010,LOWORD(opr->DOPERAND));
		break;

	default:
		error2(BUG);
		break;
	}
	locntr += oprsize / 8;
}


/**********************************************************************/
/*   Rcollectopcode guarantees that the relative address is within    */
/*        the proper range.                                           */
/**********************************************************************/

rcollectopcode (opr, bitcount)

register OPERANDDESC *opr;
register int bitcount;

{
	register unsigned maskval;
	register unsigned val;

	val = LOWORD(opr->DOPERAND);

	maskval = 0XFFFF;
	maskval <<= bitcount - 1;

	if (((val & maskval) != 0) && ((val & maskval) != maskval))
		error2(RELADDR);

	collectopcode (val, bitcount);
}

/**********************************************************************/
/*   Bcollectopcode guarantees that the relative address is within    */
/*        the proper range (for negative values).                     */
/**********************************************************************/

bcollectopcode (opr, bitcount)

register OPERANDDESC *opr;
register int bitcount;

{
	register unsigned maskval;
	register unsigned val;

	val = LOWORD(opr->DOPERAND);

	maskval = 0XFFFF;
	maskval <<= bitcount;

	if ((val & maskval) != 0)
		error2(RELADDR);

	collectopcode (val, bitcount);
}


/**********************************************************************/
/*   Xcollectopcode guarantees that the value being output is within  */
/*        the required range.                                         */
/**********************************************************************/

xcollectopcode (opr, bitcount)

register OPERANDDESC *opr;
register int bitcount;

{
	register unsigned maskval;
	register unsigned val;

	val = LOWORD(opr->DOPERAND);

	maskval = 0XFFFF;
	maskval <<= bitcount;

	if (((val & maskval) != 0) && ((val & maskval) != maskval))
		error2(CONSTERR);

	collectopcode (val, bitcount);
}


/**********************************************************************/
/*   Fcheck insures that the proper operand has been used for a	      */
/*	floating point instruction.  Only rq0,rq4,rr2,rr6,r3, and     */
/*	r7 are allowed.  The size is the number of words the          */
/*	the operand designates.					      */
/**********************************************************************/

fcheck (opr, size)

register OPERANDDESC *opr;
register int size;

{
	register int retval;

	retval = LOWORD(opr->DOPERAND) - (4-size);

	if (retval % 4)
		error2(ILLOPER);
	else
		return((retval/4)? 0 : 1);
}


/**********************************************************************/
/*   Collectopcode collects bits in opcodeword, outputting the value  */
/*        if necessary (i.e., if a 16 bit boundary is reached).       */
/**********************************************************************/

collectopcode (val, bitcount)

register int val;
register int bitcount;

{
	register unsigned andmask;

	andmask = 0XFFFF;
	andmask >>= 16 - bitcount;
	val &= andmask;

	if (16 - bitsused < bitcount) {
		error2(BUG);
		bitsused = 16;
	}
	else
		bitsused += bitcount;

	opcodeword |= val << (16 - bitsused);

	outputcheck();
}


/**********************************************************************/
/*   Outputcheck outputs opcodeword if a 16 bit boundary is reached.  */
/**********************************************************************/

outputcheck () {

	if (bitsused == 16) {
		putword(N_ABS,opcodeword);
		locntr += 2;
		bitsused = 0;
		opcodeword = 0;
	}
}


