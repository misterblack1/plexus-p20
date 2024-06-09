#
/*****************************************************************/
/*   The scanner module is responsible for all source scanning   */
/*        and symbol table manipulation.  While scanning the     */
/*        source, new symbols are entered into the table, and    */
/*        the various tokens are seperated and returned via      */
/*        global variables.                                      */
/*****************************************************************/


#include <stdio.h>
#include "common.h"
#include "error.h"
#include "scanner.h"
#include "symbol.h"
#include "symtab.h"
#include "token.h"
#include "types.h"

#include "extern.h"
#include "extern1.h"

char * realloc();


/*****************************************************************/
/*   Class is an array of characters for all (128) ASCII bytes.  */
/*****************************************************************/

char class [128] = {

		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		DLM_CLS,       /* \b */
		DLM_CLS,       /* \t */
		OPR_CLS,       /* \n */
		ERR_CLS,
		DLM_CLS,       /* \f */
		DLM_CLS,       /* \r */
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		ERR_CLS,
		DLM_CLS,       /* space */
		OPR_CLS,       /* ! */
		STR_CLS,       /* " */
		ERR_CLS,       /* # */
		OPR_CLS,       /* $ */
		OPR_CLS,       /* % */
		OPR_CLS,       /* & */
		CHR_CLS,       /* ' */
		OPR_CLS,       /* ( */
		OPR_CLS,       /* ) */
		OPR_CLS,       /* * */
		OPR_CLS,       /* + */
		OPR_CLS,       /* , */
		OPR_CLS,       /* - */
		ID_CLS,        /* . */
		OPR_CLS,       /* / */
		DIG_CLS,DIG_CLS,DIG_CLS,DIG_CLS,DIG_CLS,     /* 0 to 4 */
		DIG_CLS,DIG_CLS,DIG_CLS,DIG_CLS,DIG_CLS,     /* 5 to 9 */
		OPR_CLS,       /* : */
		OPR_CLS,       /* ; */
		OPR_CLS,       /* < */
		OPR_CLS,       /* = */
		OPR_CLS,       /* > */
		OPR_CLS,       /* ? */
		ERR_CLS,       /* @ */
		A_F_CLS,A_F_CLS,A_F_CLS,A_F_CLS,A_F_CLS,A_F_CLS,
		ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,
		ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,
		ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,
					/* upper case a to z */
		OPR_CLS,       /* [ */
		ERR_CLS,       /* \ */
		OPR_CLS,       /* ] */
		OPR_CLS,       /* ^ */
		ID_CLS,        /* _ */
		ERR_CLS,
		A_F_CLS,A_F_CLS,A_F_CLS,A_F_CLS,A_F_CLS,A_F_CLS,
		ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,
		ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,
		ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,ID_CLS,
					/* lower case a to z */
		OPR_CLS,       /* { */
		OPR_CLS,       /* | */
		OPR_CLS,       /* } */
		ID_CLS,        /* ~ */
		EOF_CLS,       /* end of file */
};


char voc01 [] = { EXCLAMV, 26,
	1,   '!',
	1,   '%',
	1,   '&',
	1,   '(',
	1,   ')',
	1,   '*',
	1,   '+',
	1,   ',',
	1,   '.',
	1,   '/',
	1,   ':',
	1,   ';',
	1,   '<',
	1,   '=',
	1,   '>',
	1,   '?',
	1,   '[',
	1,   '\n',
	1,   ']',
	1,   '^',
	1,   '{',
	1,   '|',
	1,   '}',
	1,   '~',
	1,   '-',
	1,   '$'
};


char voc02 [] = { COMSTARTV, 14,

	2,   '/', '*',
	2,   '\n','+',
	2,   '-', '>',
	2,   '+', '+',
	2,   '-', '-',
	2,   '>', '>',
	2,   '<', '<',
	2,   '<', '=',
	2,   '>', '=',
	2,   '=', '=',
	2,   '!', '=',
	2,   '&', '&',
	2,   '|', '|',
	2,   '/', '/'
};


char symclass [] = {

		RESWORD,            /* LASTRESWORD */
		CONST,              /* CONSTVAR */
		CONST,              /* NUM */
		CONST,              /* STR */
		CONST,              /* FIELD */
		IDENT,              /* VAR */
		TYPEID,             /* ARRAYTYPE */
		TYPEID,             /* RECORDTYPE */
		TYPEID,             /* TYPEVAR */
		SUPPL,              /* INDEX */
		SUPPL,              /* FIELDESC */
		SUPPL,              /* ENDSYMTAB */
		SUPPL               /* EOFV */
	};


/*****************************************************************/
/*   Scan breaks up the source program into tokens (tok).        */
/*        Each token has a type (typ).  If a symbol is found,    */
/*        its value fields (location, section, etc.) are         */
/*        returned in section, stypeptr, and slocation.            */
/*****************************************************************/

scan () {

	long longval;

	rescan:

		*tokstr = 0;
		bufptr = tokstr + 1;

		switch (class[nxtch]) {

		case DLM_CLS:
			gchar();
			goto rescan;

		case ID_CLS:
		case A_F_CLS:
			do {
				concat();
				gchar();
			}
			while (class[nxtch] >= A_F_CLS);

			addsymbol();

			symptr = getsym(curid);
			if (symptr->STYPE == CWORD)
				tok = symptr->SLOCATION;
			else
				tok = symptr->STYPE & BYTEMASK;
			section = symptr->SECTNUM;
			slocation = symptr->SLOCATION;
			stypeptr = symptr->STYPEPTR;
			break;

		case OPR_CLS:
			concat();
			gchar();

			if (class[nxtch] == OPR_CLS) {
				concat();
				tok = getindex(voc02) & BYTEMASK;
				slocation = tok;
				if (tok == CONTINUEV) {
					gchar();
					goto rescan;
				}

				if (tok == COMSTARTV) {
					gchar();
					while (TRUE) {
						if (nxtch == '*') {
							gchar();
							if (nxtch == '/') {
								gchar();
								break;
							}
						}
						else if (class[nxtch] == EOF_CLS)
							break;
						else
							gchar();
					}
					goto rescan;
				}

				if (tok == LINECOMV) {
					gchar();
					while (TRUE) {
						if (nxtch == '\n') {
							gchar();
							if (nxtch == '+') {
								gchar();
								continue;
							}
							tok = NEWLINEV;
							break;
						}
						if (class[nxtch] == EOF_CLS)
							break;
						gchar();
					}
					break;
				}

				if (tok != UNDEF) {
					gchar();
					break;
				}
				bufptr--;
				(*tokstr)--;
			}

			tok = getindex(voc01) & BYTEMASK;
			slocation = tok;

			if (tok == UNDEF) {
				error1(ILLOP);
				goto rescan;
			}
			break;


		case DIG_CLS:
			tok = NUM;
			if (nxtch == '0') {
				gchar();
				if ((nxtch == 'X') || (nxtch == 'x')) {
					gchar();
					num(16,&longval);
				}
				else if ((nxtch == 'B') || (nxtch == 'b')) {
					gchar();
					num(2,&longval);
				}
				else
					num(8,&longval);
			}
			else
				num(10,&longval);

			stypeptr = HIWORD(longval);
			slocation = LOWORD(longval);
			break;

		case STR_CLS:
			gchar();

			while ((class[nxtch] != STR_CLS) && (class[nxtch] != EOF_CLS)) {
				if (nxtch == '\\')
					specialtext();
				else {
					concat();
					gchar();
				}
			}

			tok = STR;
			gchar();
			stypeptr = 0;
			slocation = 0;
			break;

		case CHR_CLS:
			gchar();
			tok = NUM;

			stypeptr = 0;
			slocation = 0;
			while ((class[nxtch] != CHR_CLS) && (class[nxtch] != EOF_CLS)) {
				if (nxtch == '\\')
					specialtext();
				else {
					concat();
					gchar();
				}
			}
			gchar();

			switch (*tokstr) {

			case 0:
				break;

			case 1:
				slocation = tokbuf[1];
				break;

			case 2:
				slocation = tokbuf[1];
				slocation <<= 8;
				slocation += tokbuf[2];
				break;

			case 3:
				stypeptr = tokbuf[1];
				slocation = tokbuf[2];
				slocation <<= 8;
				slocation += tokbuf[3];
				break;

			default:
				error1(ILLNUM);

			case 4:
				stypeptr = tokbuf[1];
				stypeptr <<= 8;
				stypeptr += tokbuf[2];
				slocation = tokbuf[3];
				slocation <<= 8;
				slocation += tokbuf[4];
				break;
			}
			break;

		case ERR_CLS:
			eolerror1(ILLCHAR);
			gchar();
			goto rescan;

		default:
			tok = EOFV;
			break;
		}

	if (((tok & BYTEMASK) > LASTRESWORD) && ((tok & BYTEMASK) <= LASTNONRESWORD))
		typ = symclass[tok - LASTRESWORD] & BYTEMASK;
	else
		typ = RESWORD;

	if (p1trace == TRUE)
		printf("\nSCAN -- %x, %x, %x, %x, %x",typ,tok,section,
			stypeptr,slocation);
}


gettok(val) {

	register rtok;

	switch(val) {
	case '!': rtok = EXCLAMV; break;
	case '%': rtok = PERCENTV; break;
	case '&': rtok = AMPERV; break;
	case '(': rtok = LPARENV; break;
	case ')': rtok = RPARENV; break;
	case '*': rtok = STARV; break;
	case '+': rtok = PLUSV; break;
	case ',': rtok = COMMAV; break;
	case '/': rtok = SLASHV; break;
	case ':': rtok = COLONV; break;
	case ';': rtok = SEMIV; break;
	case '<': rtok = LESSV; break;
	case '=': rtok = EQV; break;
	case '>': rtok = GTRV; break;
	case '?': rtok = QUESTIONV; break;
	case '[': rtok = LBRACKV; break;
	case '\n': rtok = NEWLINEV; break;
	case ']': rtok = RBRACKV; break;
	case '^': rtok = CARETV; break;
	case '{': rtok = LBRACEV; break;
	case '|': rtok = VBARV; break;
	case '}': rtok = RBRACEV; break;
	case '~': rtok = TILDEV; break;
	case '-': rtok = MINUSV; break;
	case '$': rtok = DOLLARV; break;

	case '/*': rtok = COMSTARTV; break;
	case '\n+': rtok = CONTINUEV; break;
	case '->': rtok = RTARROWV; break;
	case '++': rtok = PLUSPLUSV; break;
	case '--': rtok = MINMINV; break;
	case '>>': rtok = SHRV; break;
	case '<<': rtok = SHLV; break;
	case '<=': rtok = LESSEQV; break;
	case '>=': rtok = GTREQV; break;
	case '==': rtok = EQEQV; break;
	case '!=': rtok = NOTEQV; break;
	case '&&': rtok = AMPERAMPERV; break;
	case '||': rtok = VBARVBARV; break;
	case '//': rtok = LINECOMV; break;
	}
	return(rtok);
}


/************************************************************************/
/*   Gchar gets a single character from the source file and leaves it	*/
/*	in the variable nxtch.						*/
/************************************************************************/

gchar () {

	if (nxtch == '\n') eols++;

	if ((nxtch = getc(inbuf)) < 0) nxtch = 0X7F;

	if (verbose == TRUE) putchar(nxtch);
}


/*****************************************************************/
/*   Grparen looks for a right paren and skips past it.  (An     */
/*        error is generated if it isn't found.)                 */
/*****************************************************************/

grparen () {
		
	gchar();
	if (nxtch != ')')
		error1(NORPAREN);
	else
		gchar();
}


/*****************************************************************/
/*   Getindex returns the index (tok encoding) of tokstr.        */
/*        The search starts in vocab from vocptr.                */
/*****************************************************************/

getindex (vocptr)

char *vocptr;

{
	unsigned index;
	int count;
	int length;

	index = *vocptr++;
	count = *vocptr++;
	length = *vocptr + 1;

	while (count-- > 0) {
		if (strncmp (vocptr,tokstr,*vocptr+1) == 0)
			return(index);
		index++;
		vocptr += length;
	}
	return (UNDEF);
}


/*****************************************************************/
/*   Concat adds nxtch to the end of tokstr.  Bufptr points      */
/*        to the next available location.                        */
/*****************************************************************/

concat () {

	if (*tokstr == -1) {
		error1(TOOLONG);
		tokstr = 0;
		bufptr = tokstr + 1;
	}
	else {
		*bufptr++ = nxtch;
		(*tokstr)++;
	}
}


/*****************************************************************/
/*   Specialtext processes escape sequence values (those not     */
/*        representable in normal ASCII).  They consist of a     */
/*        backslash followed by either a special character or    */
/*        an octal sequence of digits.                           */
/*****************************************************************/

specialtext () {

	int i, val, newval;
	char savech;

	gchar();

	switch (nxtch) {

	case 'a':
	case 'A':
		nxtch = '\006';
		break;


	case 'b':
	case 'B':
		nxtch = '\b';
		break;

	case 'e':
	case 'E':
		nxtch = '\004';
		break;

	case 'f':
	case 'F':
		nxtch = '\f';
		break;

	case 'n':
	case 'N':
		nxtch = '\n';
		eols--;
		break;

	case 'o':
	case 'O':
		nxtch = '\o';
		break;

	case 'p':
	case 'P':
		nxtch = '\033';
		break;

	case 'r':
	case 'R':
		nxtch = '\r';
		break;

	case 't':
	case 'T':
		nxtch = '\t';
		break;

	case 's':
	case 'S':
		nxtch = ' ';
		break;

	case '\\':
		nxtch = '\\';
		break;

	case '\"':
		nxtch = '\"';
		break;

	case '\'':
		nxtch = '\'';
		break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		val = 0;
		for (i=0; i<3; i++) {
			if ((newval = evalnum(8)) == UNDEF)
				break;
			val *= 8;
			val += newval;
			gchar();
		}
		savech = nxtch;
		nxtch = val;
		concat();
		nxtch = savech;
		return;
	}
	concat();
	gchar();
}


/*****************************************************************/
/*   Strcompr compares two strings (pointed to by vocptr and     */
/*        tokstr) for equality.  Both have a leading length      */
/*        byte.                                                  */
/*****************************************************************/

strcompr (vocptr)

char *vocptr;

{

	int size;
	char *ptr;

	size = *vocptr + 1;
	ptr = tokstr;

	while (size-- > 0)
		if (*ptr++ != *vocptr++)
			return (FALSE);

	return (TRUE);
}


/*****************************************************************/
/*   Num parses a number of the given base.  It stops parsing    */
/*        when an invalid character or number is encountered.    */
/*****************************************************************/

num (base, longval)

int base;
long *longval;

{
	int newval;
	int val;

	if ((val = evalnum(base)) == UNDEF) {
		*longval = 0;
		return;
	}

	*longval = val;
	gchar();

	while ((newval = evalnum(base)) != UNDEF) {
		*longval *= base;
		*longval += newval;
		gchar();
	}

	return;
}


/*****************************************************************/
/*   Evalnum evaluates nxtch to determine if it is in the        */
/*        range [0-9], [A-F], or [a-f].  If so, the              */
/*        corresponding hex value is generated.  It is then      */
/*        checked against the number base.                       */
/*****************************************************************/

evalnum (base)

int base;

{
	unsigned val;

	if (class[nxtch] == DIG_CLS)
		val = nxtch - '0';
	else if (class[nxtch] == A_F_CLS)
		val = (nxtch & 0X4F) - 55;
	else return (UNDEF);

	if (base <= val)
		return (UNDEF);
	return (val);
}


/*****************************************************************/
/*   Addsymbol looks up the symbol stored in the buffer pointed  */
/*        to by tokstr.  If found, it returns the symbol table   */
/*        offset of that symbol.  If not found, it is inserted   */
/*        into the symbol table, its offset is inserted into     */
/*        the hash table, and its offset is returned.            */
/*****************************************************************/

addsymbol () {

	unsigned hashpos;
	
	curid = hashtab[hashpos = lookup()];

	if (curid == EMPTY) {
		curid = nextpos;
		symptr = getsym(curid);
		symptr->STYPE = VAR;
		symptr->SECTNUM = 0;
		symptr->SLOCATION = 0;
		insert(hashpos);
	}
	if (p1trace == TRUE)
		printf("\nSYMBOL -- %s %d, %d, %d, %d, %c",symptr,symptr->STYPE,
			symptr->SECTNUM,symptr->STYPEPTR,
			symptr->SLOCATION,symptr->NAME[0]);
	return (curid);
}


/*****************************************************************/
/*   Lookup hashes the symbol, checks for a match through the    */
/*        hash table, rehashes and repeats if necessary, and     */
/*        returns when a match is found, or if an empty slot is  */
/*        found.                                                 */
/*****************************************************************/

lookup () {

	int rehashcount;
	int result;
	int pos;

	pos = random(tokstr);

	rehashcount = 0;

	while (hashtab[pos] != EMPTY) {
		if (compare(pos) == SUCCESS)
			return (pos);

		pos = rehash(pos,tokstr);
		rehashcount += 1;

		if (rehashcount == HASHTABSIZE)
			faterror(HASHTABLE);
	}
	return (pos);
}


/*****************************************************************/
/*   Compare compares the symbol found through the hash table    */
/*        with offset pos with the symbol in tokstr.  SUCCESS    */
/*        or FAILURE is returned.                                */
/*****************************************************************/

compare (pos)

int pos;

{

	int siz;
	char *ptr1;
	char *ptr2;
	unsigned uppermask;

	if (hashtab[pos] == NOTFULL)
		return (FAILURE);

	symptr = getsym(hashtab[pos]);
	ptr1 = &symptr->SIZE;
	ptr2 = tokstr;

	siz = *ptr1;
	if (*ptr1++ != *ptr2++)
		return (FAILURE);

	if (((*ptr2 >= 'a' && *ptr2 <= 'z') || *ptr2 == '.') 
		&& hashtab[pos] <= firstid)
		uppermask = 0XDF;
	else
		uppermask = 0XFF;

	while (siz-- > 0) {
		if (*ptr1 >= 'A' && *ptr1 <= 'Z') {
			if (*ptr1++ == (*ptr2++ & uppermask))
				continue;
		}
		else if (*ptr1++ == *ptr2++)
			continue;
		return (FAILURE);
	}
	return(SUCCESS);
}


/*****************************************************************/
/*   Insert inserts the symbol offset in the hash table, and     */
/*        then copies the symbol name REGISTER into the symbol table      */
/*        storage.  Symbol table overflow is checked.            */
/*****************************************************************/

insert (pos)

int pos;

{

	int size;
	char *ptr1;
	char *ptr2;

	if (tokstr[1] != '~')
		hashtab[pos] = nextpos;
	symptr = getsym(nextpos);
	size = *tokstr;

	for (ptr1 = tokstr, ptr2 = &symptr->SIZE; size >= 0; size--)
		*ptr2++ = *ptr1++;

	nextpos += *tokstr + sizeof (SYMBOL) - 1;
	if (nextpos%2 == 1) nextpos++;
}



/*****************************************************************/
/*   Random hashes the symbol pointed to by ptr.  The characters */
/*        are added to the alternate bytes of a word, and the    */
/*        result is taken modulo the hash table size.            */
/*****************************************************************/

random (ptr)

char *ptr;

{
	int i;
	int size;
	unsigned *wordptr;
	unsigned total;

	size = *ptr;

	if (size%2 == 0)
		ptr[size + 1] = 0;

	total = 0;

	for (i=0; i<=size; i+=2) {
		wordptr = (unsigned *)&ptr[i];
		total += *wordptr & 0XDFDF;
	}

	return (total % HASHTABSIZE);
}


/*****************************************************************/
/*   Rehash simply adds a constant to the previous hash value.   */
/*****************************************************************/

rehash (oldpos, strptr)

unsigned oldpos;
char *strptr;

{

	return ((oldpos + 47) % HASHTABSIZE);
}


/*****************************************************************/
/*   Hash is the debug version of random -- always returns 0.    */
/*****************************************************************/

hash (ptr)

char *ptr;

{

	return (0);
}


/************************************************************************/
/*   Getsym returns the address of the requested symbol.		*/
/*									*/
/*	When the symbol table to disk scheme is implemented, this	*/
/*	routine will get more complicated.				*/
/************************************************************************/

SYMBOL *getsym (symid)

unsigned symid;

{
	long id;
	long idplus;

	id = symid;
	id -= firstid;
	idplus = id + MAXSTRINGSIZE + sizeof *symptr;

	if (idplus > freespace) {
		freestorage = realloc (freestorage, freespace += 2046);
		if (freestorage == NULL)
			faterror (NOSPACE);
		}

	if (symid < firstid)
		return ((SYMBOL *)&symtab[symid]);
	else
		return ((SYMBOL *)(freestorage + id));
}

