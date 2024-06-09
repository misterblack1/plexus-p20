#
/*        involved, it is evaluated, checking to make sure that the   */
/*        expression is valid.  Getval is called to return a single   */
/*        element of the operand, getexpr is called to return a       */
/*        subexpression.  If getval encounters a binary operator,     */
/*        it simply returns to its caller with the token value        */
/*        of that operator.                                           */
/**********************************************************************/


#include <a.out.h>
#include <stdio.h>
#include "common.h"
#include "error.h"
#include "ifile.h"
#include "mask.h"
#include "symbol.h"
#include "symtab.h"
#include "token.h"
#include "types.h"

#include  "extern.h"
#include  "extern2.h"


/************************************************************************/
/*   Getoperand gets the requested operand.				*/
/************************************************************************/

getoperand (opnum)

register int opnum;
{

	getval(&oprand[opnum].DTYPE);
}


/**********************************************************************/
/*   Getval generates a single value from the IFILE.  If it is an     */
/*        expression, it is evaluated and its value is returned in    */
/*        opr.  Unary operations are performed.  SUCCESS is returned  */
/*        if a value was encountered, the operator token is returned  */
/*        if an operator is encountered.                              */
/**********************************************************************/

getval (opr)

register OPERANDDESC *opr;

{
	OPERANDDESC opr2;

	opr->DTYPE = N_ABS;
	opr->DISPLAC = 0;
	opr->DOPERAND = 0;

	while (TRUE) {

		switch (itag) {

		case DISP:
			itag = getbyte();
			getval(&opr2);
			if (opr->DTYPE != N_ABS)
				opr->DOPERAND += opr2.DOPERAND;
			else
				opr->DOPERAND += opr2.DOPERAND;
			itag = getbyte();
			continue;
		}
	break;
	}


	switch (itag) {

	case EXPR:
		itag = getbyte();
		getval(opr);
		getexpr(opr);
		if (itag != ENDEXPR)
			error2(BUG);
		itag = getbyte();
		break;

	case LDBYTE:
		LOWORD(opr->DOPERAND) = getbyte();
		itag = getbyte();
		break;

	case LDWORD:
		LOWORD(opr->DOPERAND) = getword();
		itag = getbyte();
		break;

	case LDLONG:
		HIWORD(opr->DOPERAND) = getword();
		LOWORD(opr->DOPERAND) = getword();
		itag = getbyte();
		break;

	case EXREF:
		symptr = getsym(getword());
		opr->DTYPE = symptr->SECTNUM;
		if (symptr->SECTNUM == N_EXT)
			opr->DISPLAC = symptr->STYPEPTR;
		else {
			opr->DOPERAND = (int) symptr->SLOCATION;
			LOWORD(opr->DOPERAND) = symptr->SLOCATION;
		}

		itag = getbyte();
		break;

	case LOCDOT:
		opr->DTYPE = sc;
		opr->DOPERAND = locntr;
		itag = getbyte();
		break;

	default:
		error2(BUG);
		itag = getbyte();
		break;
	}


	while (TRUE) {

		switch (itag) {

		case UMINV:
			if ((opr->DTYPE & ~N_EXT) != N_ABS)
				error2(ILLCEXPR);
			else
				opr->DOPERAND = -opr->DOPERAND;
			itag = getbyte();
			continue;

		case UPLUSV:
			itag = getbyte();
			continue;

		case TILDEV:
			if ((opr->DTYPE & ~N_EXT) != N_ABS)
				error2(ILLCEXPR);
			else
				opr->DOPERAND = ~opr->DOPERAND;

			itag = getbyte();
			continue;

		case EXCLAMV:
			if ((opr->DTYPE & ~N_EXT) != N_ABS)
				error2(ILLCEXPR);
			else
				opr->DOPERAND = !opr->DOPERAND;

			itag = getbyte();
			continue;
		}
	break;
	}

}


/**********************************************************************/
/*   Getexpr gets an expression from the IFILE.  It assumes the first */
/*        operand is in opr and processes binary operators as long as */
/*        they are encountered.  If another level is found, a         */
/*        recursive call is made.                                     */
/**********************************************************************/

getexpr (opr)

register OPERANDDESC *opr;

{
	OPERANDDESC opr2;
	register long *op1;
	register long *op2;
	register int op1tag;
	register int op2tag;

	op1 = &opr->DOPERAND;
	op2 = &opr2.DOPERAND;

	while (itag != ENDEXPR) {
		if (itag >= CWORD)
			return;
		getval(&opr2);


		if (itag < CWORD)
			getexpr(&opr2);

		op1tag = opr->DTYPE;
		op2tag = opr2.DTYPE;

		if (((op1tag & ~N_EXT) == N_ABS)
				&& ((op2tag & ~N_EXT) == N_ABS)) {
			switch (itag) {

			case STARV:
				*op1 *= *op2;
				break;

			case SLASHV:
				*op1 /= *op2;
				break;

			case PERCENTV:
				*op1 %= *op2;
				break;

			case PLUSV:
				*op1 += *op2;
				break;

			case MINUSV:
				*op1 -= *op2;
				break;

			case SHRV:
				*op1 >>= *op2;
				break;

			case SHLV:
				*op1 <<= *op2;
				break;

			case LESSV:
				*op1 = (*op1 < *op2) ? TRUE : FALSE;
				break;

			case GTRV:
				*op1 = (*op1 > *op2) ? TRUE : FALSE;
				break;

			case LESSEQV:
				*op1 = (*op1 <= *op2) ? TRUE : FALSE;
				break;

			case GTREQV:
				*op1 = (*op1 >= *op2) ? TRUE : FALSE;
				break;

			case EQV:
				*op1 = (*op1 == *op2) ? TRUE : FALSE;
				break;

			case NOTEQV:
				*op1 = (*op1 != *op2) ? TRUE : FALSE;
				break;

			case AMPERV:
				*op1 &= *op2;
				break;

			case CARETV:
				*op1 ^= *op2;
				break;

			case VBARV:
				*op1 |= *op2;
				break;

			case AMPERAMPERV:
				*op1 = *op1 && *op2;
				break;

			case VBARVBARV:
				*op1 = *op1 || *op2;
				break;

			default:
				error2(BUG);

			}
		itag = getbyte();
		}

		else {
			if (itag == PLUSV) {
				if ((op1tag + op2tag) == N_EXT) {
					error2(ILLCEXPR);
					continue;
				}
				if ((op1tag & ~N_EXT) == N_ABS) {
					opr->DTYPE = opr2.DTYPE;
					opr->DISPLAC = opr2.DISPLAC;
				}
				else if ((op2tag & ~N_EXT) != N_ABS) {
					error2(ILLCEXPR);
					itag = getbyte();
					continue;
				}
				*op1 += *op2;
			}
			else if (itag == MINUSV) {
				if ((op1tag & ~N_EXT) == (op2tag & ~N_EXT)) {
					if (op1tag == N_EXT) {
						error2(ILLCEXPR);
						continue;
					}
					opr->DTYPE = N_ABS;
				}
				else if ((op2tag & ~N_EXT) != N_ABS) {
					error2(ILLCEXPR);
					itag = getbyte();
					continue;
				}
				*op1 -= *op2;
			}
		itag = getbyte();
		}
	}
}
