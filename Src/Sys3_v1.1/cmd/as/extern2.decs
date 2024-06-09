
/* extern2.decs */

/* External symbols used by pass 1 of the assembler */

int  bitcount;		/* Bits in current portion of instruction      */
			/*   being processed                             */
int  bitsused;		/* Bits used in current word of instruction       */
int codesect;		/* CODE section number */
FILE *databuf;		/* Buffer for data object code */
int datasect;		/* DATA section number */
int errorbuf [MAXERRORCOUNT];	/* Error messages for current line */
int errorcount;		/* Number of errors in this line */
int extrasource;	/* Flag that an extra source line has been read */
FORMATENTRY formattable []; /* Format table */
char *ifile;		/* Name of intermediate file (IFILE) */
unsigned itag;		/* Current IFILE tag being processed by pass 2 */
int leftmargin;		/* Number of spaces to left margin */
int list;		/* Flag indicating whether listing is on or off */
FILE *listbuf;		/* Buffer for listing file */
int listcol;		/* Current column in listing line */
int locntr;		/* Current location counter for pass 2 */
int modid;		/* Module identifier (needs to initialized) */
int newlocntr;		/* New location counter */
int numcr;		/* Current number of eols not yet processed */
int numerrors;		/* Total number of errors encountered */
int objcols;		/* Number of columns designated for object code */
int obj1col;		/* Column number of first object word in listing */
int obj2col;		/* Column number of second object word in listing */
OPCODENTRY opcodetable []; /* Operation code table */
int opcodeword;		/* Current opcode word being constructed */
OPERANDDESC oprand [MAXOPCOUNT];	/* Current operands */
char operandtable [] [2]; /* Operand combination table */
OPERANDDESC *op1;	/* Pointer to first operand structure */
OPERANDDESC *op2;	/* Pointer to second operand structure */
OPERANDDESC *op3;	/* Pointer to third operand structure */
OPERANDDESC *op4;	/* Pointer to fourth operand structure */
char outwordbuf [];
int outwordcount;
unsigned datarel;	/* Number of relocated data words */
FILE *reldatabuf;	/* Buffer for data relocation information */
FILE *textbuf;		/* Buffer for text object code */
unsigned textrel;	/* Number of relocated text words */
FILE *reltextbuf;	/* Buffer for text relocation information */
char *srcfile;		/* Name of source file */
int sectctr;		/* Next available section number */
unsigned symbuf [MAXERRORCOUNT];	/* Error symbols */
SYMBOL *symptr;		/* Pointer to current symbol */
unsigned symsize;	/* Size of symbols in object file */
int undef;		/* Number of undefined symbols */
unsigned undefsym;	/* Last undefined symbol (for error messages). */

