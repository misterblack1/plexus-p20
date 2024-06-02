#
/**********************************************************************/
/*   This module contains all the code necessary to process           */
/*        expressions during pass 1.  It can be divided into two      */
/*        parts:  constant expressions (those that need to be         */
/*        evaluated during pass1), and IFILE expressions (those       */
/*        that are put into the IFILE and evaluated by pass2).        */
/*        Standard "C" operator precedence is assumed.                */
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
/*   Constant LOGICAL OR procedure.          (||)                     */
/**********************************************************************/

constexpr (val)
register long *val;

{
	register long val2;
	register int exprtyp;

	exprtyp = constlandexpr(val);

	while(tok == VBARVBARV) {
		scan();
		if ((constlandexpr(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);
		*val = *val || val2;
	}
	return (exprtyp);
}


/**********************************************************************/
/*   Constant LOGICAL AND procedure.         (&&)                     */
/**********************************************************************/

constlandexpr(val)

register long *val;

{
	register long val2;
	register int exprtyp;

	exprtyp = constorexpr(val);

	while (tok == AMPERAMPERV) {
		scan();
		if ((constorexpr(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);
		*val = *val && val2;
	}
	return (exprtyp);
}


/**********************************************************************/
/*   Constant OR procedure.             (|)                           */
/**********************************************************************/

constorexpr(val)

register long *val;

{
	register long val2;
	register int exprtyp;

	exprtyp = constxorexpr(val);

	while (tok == VBARV) {
		scan();
		if ((constxorexpr(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);
		*val |= val2;
	}
	return (exprtyp);
}


/**********************************************************************/
/*   Constant XOR procedure.            (^)                           */
/**********************************************************************/

constxorexpr(val)

register long *val;

{
	register long val2;
	register int exprtyp;

	exprtyp = constandexpr(val);

	while (tok == CARETV) {
		scan();
		if ((constandexpr(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);
		*val ^= val2;
	}
	return (exprtyp);
}


/**********************************************************************/
/*   Constant AND procedure.            (&)                           */
/**********************************************************************/

constandexpr(val)

register long *val;

{
	register long val2;
	register int exprtyp;

	exprtyp = constequexpr(val);

	while (tok == AMPERV) {
		scan();
		if ((constequexpr(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);
		*val &= val2;
	}
	return (exprtyp);
}


/**********************************************************************/
/*   Constant EQUALITY procedure.            (==,!=)                  */
/**********************************************************************/

constequexpr(val)

register long *val;

{
	register long val2;
	register int exprtyp;
	register int oprtok;

	exprtyp = constrelexpr(val);

	while ((tok == EQV) || (tok == NOTEQV)) {
		oprtok = tok;
		scan();
		if ((constrelexpr(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);

		switch (oprtok) {

		case LESSEQV :
			*val = (*val == val2);
			break;

		case NOTEQV:
			*val = (*val != val2);
			break;
		}
	}
	return(exprtyp);
}


/**********************************************************************/
/*   Constant RELATIONAL procedure.          (>,<,>=,<=)              */
/**********************************************************************/

constrelexpr(val)

register long *val;

{
	register long val2;
	register int exprtyp;
	register int oprtok;

	exprtyp = constshexpr(val);

	while ((tok == GTRV) || (tok == LESSV) || (tok == LESSEQV)
			|| (tok == GTREQV)) {
		oprtok = tok;
		scan();
		if ((constshexpr(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);

		switch (oprtok) {

		case GTRV :
			*val = (*val > val2);
			break;

		case LESSV:
			*val = (*val < val2);
			break;

		case LESSEQV:
			*val = *val <= val2;
			break;

		case GTREQV :
			*val = *val >= val2;
			break;
		}
	}
	return(exprtyp);
}


/********************************************************************/
/*   Constant SHIFT procedure.          (>>,<<)                     */
/********************************************************************/

constshexpr(val)

register long *val;

{
	register long val2;
	register int exprtyp;
	register int oprtok;

	exprtyp = constaddexpr(val);

	while ((tok == SHRV) || (tok == SHLV)) {
		oprtok = tok;
		scan();
		if ((constaddexpr(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);

		switch (oprtok) {

		case SHRV :
			*val >>= val2;
			break;

		case SHLV:
			*val <<= val2;
			break;
		}
	}
	return(exprtyp);
}


/**********************************************************************/
/*   Constant ADD procedure.            (+,-)                         */
/**********************************************************************/

constaddexpr(val)

register long *val;
{
	register long val2;
	register int exprtyp;
	register int exprtyp2;
	register int oprtok;

	exprtyp = constmulexpr(val);

	while ((tok == PLUSV) || (tok == MINUSV)) {
		oprtok = tok;
		scan();
		exprtyp2 = constmulexpr(&val2);

		if (oprtok == PLUSV) {
			if ((exprtyp == N_ABS) && (exprtyp2 != N_ABS))
				exprtyp = exprtyp2;
			else if (exprtyp2 != N_ABS)
				error1(ILLCEXPR);
			*val += val2;
		}
		else {
			if (exprtyp == exprtyp2)
				exprtyp = N_ABS;
			else if (exprtyp2 != N_ABS)
				error1(ILLCEXPR);
			*val -= val2;
		}
	}
	return (exprtyp);
}


/**********************************************************************/
/*   Constant MULTIPLY procedure.            (*,/,%)                */
/**********************************************************************/

constmulexpr(val)

register long *val;

{
	register long val2;
	register int exprtyp;
	register int oprtok;

	exprtyp = constfactor(val);

	while ((tok == STARV) || (tok == SLASHV) || (tok == PERCENTV)) {
		oprtok = tok;
		scan();
		if ((constfactor(&val2) != N_ABS) || (exprtyp != N_ABS))
			error1(ILLCEXPR);

		switch (oprtok) {

		case STARV :
			*val *= val2;
			break;

		case SLASHV:
			*val /= val2;
			break;

		case PERCENTV:
			*val = *val % val2;
			break;
		}
	}
	
	return(exprtyp);
}


/**********************************************************************/
/*   Constfactor evaluates a single factor of a constant              */
/*        expression and returns its type (e.g., constant,            */
/*        relocatable, symbol reference).                             */
/**********************************************************************/

constfactor(val)

register long *val;

	{

	char exprtyp;

	switch (typ) {

	case RESWORD:
		switch (tok) {

		case LPARENV:
			scan();		/* Scan past '(' */
			exprtyp = constexpr(val);
			if (tok != RPARENV) {
				error1(NORPAREN);
				exprtyp = N_UNDF;
			}
			scan();		/* Scan past '(' */
			break;

		case MINUSV:
			scan();		/* Scan past '-' */
			exprtyp = constfactor(val);
			*val = -(*val);
			break;

		case EXCLAMV:
			scan();		/* Scan past '!' */
			exprtyp = constfactor(val);
			*val = !(*val);
			break;

		case TILDEV:
			scan();		/* Scan past '~' */
			exprtyp = constfactor(val);
			*val = ~(*val);
			break;

		case SIZEOFV:
			scan();		/* Scan past 'sizeof' */
			exprtyp = N_ABS;
			/*
			*val = sizeofop(curid);
			*/
			break;

		case DOTV:
			scan();		/* Scan past '.' */
			*val = lc;
			exprtyp = sc;
			break;

		case DOTDOTV:
			scan();
			HIWORD(*val) = 0;
			LOWORD(*val) = rellc;
			exprtyp = N_ABS;
			break;

		case REG:
		case REGPAIR:
		case REGHILO:
		case REGQUAD:
			exprtyp = N_REG;
			HIWORD(*val) = 0;
			LOWORD(*val) = symptr->SLOCATION;
			scan();
			break;
		}
		break;

	case CONST:
		switch (tok) {

		case NUM:
		case STR:
		case CONSTVAR:
			HIWORD(*val) = stypeptr;
			LOWORD(*val) = slocation;
			scan();		/* Scan past constant */
			exprtyp = N_ABS;
		}
		break;

	case IDENT:
		HIWORD(*val) = stypeptr;
		LOWORD(*val) = slocation;
		exprtyp = section;
		scan();
		break;

	default:
		exprtyp = N_UNDF;
	}
	return (exprtyp);

}


/************************************************************************/
/*   Oprexpr brackets an IFILE expression with EXPR-ENDEXPR if		*/
/*	necessary.							*/
/************************************************************************/

oprexpr () {

	register int result;

	beginexpr = opbufptr;
	opputbyte(EXPR);
	exprflag = FALSE;

	result = oprlorexpr();

	if (exprflag == TRUE) {
		opputbyte(ENDEXPR);
		exprflag = FALSE;
	}
	else
		deloprbyte();

	return (result);
}


/**********************************************************************/
/*   IFILE LOGICAL OR procedure.             (||)                     */
/**********************************************************************/

oprlorexpr() {

	register int result;

	result = oprlandexpr();

	while (tok == VBARVBARV) {
		scan();
		result = oprlandexpr();
		opputbyte(VBARVBARV);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   IFILE LOGICAL AND procedure.            (&&)                     */
/**********************************************************************/

oprlandexpr() {

	register int result;

	result = oprorexpr();

	while (tok == AMPERAMPERV) {
		scan();
		result = oprorexpr();
		opputbyte(AMPERAMPERV);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   IFILE OR procedure.                     (|)                      */
/**********************************************************************/

oprorexpr() {

	register int result;

	result = oprxorexpr();

	while (tok == VBARV) {
		scan();
		result = oprxorexpr();
		opputbyte(VBARV);
		exprflag = TRUE;
	}
	return (result);
}

/**********************************************************************/
/*   IFILE XOR procedure.                    (^)                      */
/**********************************************************************/

oprxorexpr() {

	register int result;

	result = oprandexpr();

	while (tok == CARETV) {
		scan();
		result = oprandexpr();
		opputbyte(CARETV);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   IFILE AND procedure.                    (&)                      */
/**********************************************************************/

oprandexpr() {

	register int result;

	result = oprequexpr();

	while (tok == AMPERV) {
		scan();
		result = oprequexpr();
		opputbyte(AMPERV);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   IFILE EQUALITY procedure.               (==,!=)                  */
/**********************************************************************/

oprequexpr() {

	char oprtok;
	register int result;

	result = oprrelexpr();

	while ((tok == LESSEQV) || (tok == NOTEQV)) {
		oprtok = tok;
		scan();
		result = oprrelexpr();
		opputbyte(oprtok);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   IFILE RELATIONAL procedure.             (>,<,>=,<=)              */
/**********************************************************************/

oprrelexpr() {

	char oprtok;
	register int result;

	result = oprshexpr();

	while ((tok == GTRV) || (tok == LESSV) || (tok == LESSEQV)
			|| (tok == GTREQV)) {
		oprtok = tok;
		scan();
		result = oprshexpr();
		opputbyte(oprtok);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   IFILE SHIFT procedure.                  (>>,<<)                  */
/**********************************************************************/

oprshexpr() {

	char oprtok;
	register int result;

	result = opraddexpr();

	while ((tok == SHRV) || (tok == SHLV)) {
		oprtok = tok;
		scan();
		result = opraddexpr();
		opputbyte(oprtok);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   IFILE ADD procedure.                    (+,-)                    */
/**********************************************************************/

opraddexpr() {

	char oprtok;
	register int result;

	result = oprmulexpr();

	while ((tok == PLUSV) || (tok == MINUSV)) {
		oprtok = tok;
		scan();
		result = oprmulexpr();
		opputbyte(oprtok);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   IFILE MULTIPLY procedure.               (*,/,MOD)                */
/**********************************************************************/

oprmulexpr() {

	char oprtok;
	register int result;

	result = oprfactor();

	while ((tok == STARV) || (tok == SLASHV) || (tok == PERCENTV)) {
		oprtok = tok;
		scan();
		result = oprfactor();
		opputbyte(oprtok);
		exprflag = TRUE;
	}
	return (result);
}


/**********************************************************************/
/*   Oprfactor evaluates a single factor of an IFILE expression.      */
/*        SUCCESS or FAILURE is returned, depending on whether a      */
/*        valid expression is found.                                  */
/**********************************************************************/

oprfactor () {

	char oprtok;
	register int result;

	switch (typ) {

	case RESWORD:

		switch (tok) {

		case PLUSV:
		case MINUSV:
			tok = (tok == PLUSV) ? UPLUSV : UMINV;
		case EXCLAMV:
		case TILDEV:
		case SIZEOFV:
			oprtok = tok;
			scan();
			result = oprfactor();
			opputbyte(oprtok);
			exprflag = TRUE;
			break;

		case DOTV:
			opputbyte(LOCDOT);
			scan();
			break;

		case DOTDOTV:
			opputbyte(LDWORD);
			opputword(rellc);
			scan();
			break;

		case LPARENV:
			scan();
			result = oprlorexpr();
			if (tok != RPARENV) {
				error1(NORPAREN);
				return(FAILURE);
			}
			scan();
			break;

		default:
			return(FAILURE);
		}
		break;

	case CONST:
		if (stypeptr != 0) {
			opputbyte(LDLONG);
			opputword(stypeptr);
			opputword(slocation);
		}
		else if (slocation > 255) {
			opputbyte(LDWORD);
			opputword(slocation);
		}
		else {
			opputbyte(LDBYTE);
			opputbyte(slocation);
		}
		scan();
		break;

	case IDENT:
		symptr = getsym(curid);
		opputbyte(EXREF);
		opputword(curid);
		scan();			/* Scan past identifier */
		break;

	default:
		return (FAILURE);
	}
	return (SUCCESS);
}
