
/* extern.decs */

/* External symbols used by both passes of the assembler */

int abortflag;			/* Flag indicating abort before completion */
int	forceobject;		/* Force object file even if errors occur */
unsigned bsslc;			/* Bss location counter */
unsigned datalc;		/* Data location counter */
extern errno;			/* System error number */
char *errmsg [];		/* Error messages output by the assembler */
char *extenptr;			/* Pointer to source character extension */
unsigned firstid;		/* First identifier in symbol table */
/*extern fout;			/* Standard output value */
int freespace;			/* Amount of overflow symbol table space */
char *freestorage;		/* Pointer to overflow symbol table space */
FILE *ifilebuf;			/* Intermediate file buffer descriptor */
FILE *inbuf;			/* Source file input buffer descriptor */
unsigned linenum;		/* Current line number */
int listopt;			/* List file option */
int margin;			/* Flag indicating a left margin on listing */
int narrow;			/* Flag indicating a narrow listing */
unsigned nextpos;		/* Next identifier in symbol table */
char *outfile;			/* Output file name */
char p1trace;			/* Flag indicating pass 1 is to be traced */
char p2trace;			/* Flag indicating pass 2 is to be traced */
int rellc;			/* Relocation counter */
unsigned sc;			/* Current section counter */
int split;			/* Split instruction and data space */
char *srcfile;			/* Pointer to source file name */
int textbyte;			/* Flag indicating odd text size */
unsigned textlc;		/* Text location counter */
char *tmpfile;			/* Temporary file name ("/usr/tmp/zas...") */
int uflag;			/* Flag indicating undefineds become extern */
int verbose;			/* Verbose flag for debugging purposes */
int xref;			/* Flag indicating cross reference pass */
int zcc;			/* Flag indicating source is from zcc */

extern SYMBOL *getsym();

