
/* types.decs */

/* Type declarations */

typedef struct {
	char STYPE;			/* Type of symbol */
	char SECTNUM;			/* Section number */
	unsigned STYPEPTR;		/* Pointer to type */
	unsigned SLOCATION;		/* Location */
	char SIZE;			/* Size of name */
	char NAME [1];			/* Name string */
} SYMBOL;


typedef struct {
	char BYTECOUNT;			/* Number of bytes in instruction */
	char FIELDS [MAXFORMATSIZE];	/* Fields of instruction */
} FORMATENTRY;


typedef struct {
	unsigned OPECODE;		/* Opcode for instruction */
	char OPERAND;			/* Operand combination */
	char IFORMAT;			/* Format of instruction */
} OPCODENTRY;


typedef struct {
	unsigned DTYPE;			/* Type of operand */
	long DOPERAND;			/* Operand value */
	unsigned DISPLAC;		/* Displacement */
} OPERANDDESC;
 

typedef struct {
	int fildes;			/* File descriptor */
	int nleft;			/* Chars leeft in buffer */
	char *nextp;			/* Ptr to next character */
	char buff [512];		/* The buffer */
} IOBUF;

