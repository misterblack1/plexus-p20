/*  */
/* @(#)a.out.h	5.1 4/22/86 */

/*	@(#)a.out.h	2.4	*/

#include <nlist.h>	/* included for all machines */

#if u3b || vax || u3b5 || m68

 /*		COMMON OBJECT FILE FORMAT



 	File Organization:

 	_______________________________________________    INCLUDE FILE
 	|_______________HEADER_DATA___________________|
 	|					      |
 	|	File Header			      |    "filehdr.h"
 	|.............................................|
 	|					      |
 	|	Auxilliary Header Information	      |	   "aouthdr.h"
 	|					      |
 	|_____________________________________________|
 	|					      |
 	|	".text" section header		      |	   "scnhdr.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section header		      |	      ''
 	|					      |
 	|.............................................|
 	|					      |
 	|	".bss" section header		      |	      ''
 	|					      |
 	|_____________________________________________|
 	|______________RAW_DATA_______________________|
 	|					      |
 	|	".text" section data (rounded to 4    |
 	|				bytes)	      |
 	|.............................................|
 	|					      |
 	|	".data" section data (rounded to 4    |
 	|				bytes)	      |
 	|_____________________________________________|
 	|____________RELOCATION_DATA__________________|
 	|					      |
 	|	".text" section relocation data	      |    "reloc.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section relocation data	      |       ''
 	|					      |
 	|_____________________________________________|
 	|__________LINE_NUMBER_DATA_(SDB)_____________|
 	|					      |
 	|	".text" section line numbers	      |    "linenum.h"
 	|					      |
 	|.............................................|
 	|					      |
 	|	".data" section line numbers	      |	      ''
 	|					      |
 	|_____________________________________________|
 	|________________SYMBOL_TABLE_________________|
 	|					      |
 	|	".text", ".data" and ".bss" section   |    "syms.h"
 	|	symbols				      |	   "storclass.h"
 	|					      |
 	|_____________________________________________|
	|________________STRING_TABLE_________________|
	|					      |
	|	    long symbol names		      |
	|_____________________________________________|



 		OBJECT FILE COMPONENTS

 	HEADER FILES:
 			/usr/include/filehdr.h
			/usr/include/aouthdr.h
			/usr/include/scnhdr.h
			/usr/include/reloc.h
			/usr/include/linenum.h
			/usr/include/syms.h
			/usr/include/storclass.h

	STANDARD FILE:
			/usr/include/a.out.h    "object file" 
   */

#include "filehdr.h"
#include "aouthdr.h"
#include "scnhdr.h"
#include "reloc.h"
#include "linenum.h"
#include "syms.h"


#else /* u370 || pdp11 */


/*
 * Format of an a.out header
 */
 

struct	exec {	/* a.out header */
#if u370
	int		a_magic;	/* magic number */
	int		a_stamp;	/* The version of a.out	*/
					/* format of this file.	*/
#else
	short		a_magic;	/* magic number */
#endif
	unsigned	a_text;		/* size of text segment */
					/* in bytes		*/
					/* padded out to next	*/
					/* page boundary with	*/
					/* binary zeros.	*/
	unsigned	a_data;		/* size of initialized data */
					/* segment in bytes	*/
					/* padded out to next	*/
					/* page boundary with	*/
					/* binary zeros.	*/
	unsigned	a_bss;		/* Actual size of	*/
					/* uninitialized data	*/
					/* segment in bytes.	*/
	unsigned	a_syms;		/* size of symbol table */
	unsigned	a_entry;	/* entry point */
#if u370
	unsigned	a_trsize;	/* size of text relocation */
	unsigned	a_drsize;	/* size of data relocation */
	unsigned	a_origin;	/* The origin to which 	*/
					/* this file was	*/
					/* relocated.		*/
	unsigned	a_actext;	/* The actual size of	*/
					/* the text segment in	*/
					/* bytes.		*/
	unsigned	a_acdata;	/* The actual size of	*/
					/* the data segment in	*/
					/* bytes.		*/
#endif
#if pdp11
	char		a_unused;	/* not used */
	unsigned char	a_hitext;	/* high order text bits */
	char		a_flag;		/* reloc info stripped */
	char		a_stamp;	/* environment stamp */
#endif
};

#define	A_MAGIC1	0407		/* normal */
#define	A_MAGIC0	0401		/* lpd (UNIX/RT) */
#define	A_MAGIC2	0410		/* read-only text */
#define	A_MAGIC3	0411		/* separated I&D */
#define	A_MAGIC4	0405		/* overlay */
#define	A_MAGIC5	0437		/* system overlay, separated I&D */

#if u370
struct relocation_info {
	  long  r_address;	/* relative to current segment */
	  unsigned int
		r_symbolnum:24,	/* if extern then symbol table */
				/* ordinal (0, 1, 2, ...) else */
				/* segment number (same as symbol types) */
	        r_pcrel:1, 	/* if so, segment offset has already */
				/* been subtracted */
	  	r_length:2,	/* 0=byte, 1=word, 2=long */
	  	r_extern:1,	/* does not include value */
				/* of symbol referenced */
	  	r_offset:1,	/* already includes origin */
				/* of this segment (?) */
		r_pad:3;	/* nothing, yet */
};
#endif

/* in invocation of BADMAG macro, argument should not be a function. */

#define	BADMAG(X) (X.a_magic != A_MAGIC1 &&\
		X.a_magic != A_MAGIC2 &&\
		X.a_magic != A_MAGIC3 &&\
		X.a_magic != A_MAGIC4 &&\
		X.a_magic != A_MAGIC5 &&\
		X.a_magic != A_MAGIC0)

	/* values for type flag */

#define	N_UNDF	0	/* undefined */
#define	N_TYPE	037
#define	N_FN	037	/* file name symbol */

#if pdp11
#define	N_ABS	01	/* absolute */
#define	N_TEXT	02	/* text symbol */
#define	N_DATA	03	/* data symbol */
#define	N_BSS	04	/* bss symbol */
#define	N_REG	024	/* register name */
#define	N_EXT	040	/* external bit, or'ed in */
#define	FORMAT	"%.6o"	/* to print a value */
#else
#define	N_ABS	02	/* absolute */
#define	N_TEXT	04	/* text */
#define	N_DATA	06	/* data */
#define	N_BSS	010
#define	N_GSYM	0040	/* global sym: name,,type,0 */
#define	N_FNAME 0042	/* procedure name (f77 kludge): name,,,0 */
#define	N_FUN	0044	/* procedure: name,,linenumber,address */
#define	N_STSYM 0046	/* static symbol: name,,type,address */
#define	N_LCSYM 0050	/* .lcomm symbol: name,,type,address */
#define	N_BSTR	0060	/* begin structure: name,,, */
#define	N_RSYM	0100	/* register sym: name,,register,offset */
#define	N_SLINE	0104	/* src line: ,,linenumber,address */
#define	N_ESTR	0120	/* end structure: name,,, */
#define	N_SSYM	0140	/* structure elt: name,,type,struct_offset */
#define	N_SO	0144	/* source file name: name,,,address */
#define	N_BENUM	0160	/* begin enum: name,,, */
#define	N_LSYM	0200	/* local sym: name,,type,offset */
#define	N_SOL	0204	/* #line source filename: name,,,address */
#define	N_ENUM	0220	/* enum element: name,,,value */
#define	N_PSYM	0240	/* parameter: name,,type,offset */
#define	N_ENTRY	0244	/* alternate entry: name,,linenumber,address */
#define	N_EENUM	0260	/* end enum: name,,, */
#define	N_LBRAC	0300	/* left bracket: ,,nesting level,address */
#define	N_RBRAC	0340	/* right bracket: ,,nesting level,address */
#define	N_BCOMM	0342	/* begin common: name,,, */
#define	N_ECOMM	0344	/* end common: name,,, */
#define	N_ECOML	0350	/* end common (local name): ,,,address */
#define	N_STRU	0374	/* 2nd entry for structure: str tag,,,length */
#define	N_LENG	0376	/* second stab entry with length information */
#define	N_EXT	01	/* external bit, or'ed in */
#define	FORMAT	"%.8x"
#define	STABTYPES 0340
#endif

#endif
