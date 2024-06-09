#ifndef SRBHDR
#include	"defs.h"
#endif

UNION _printf	PRINTF;
union _printf {
	L_INT	p_expr;
	INT	p_int;
	STRING	p_string;
	CHAR	p_char;
	REAL	p_real;
	L_REAL	p_lreal;
};
