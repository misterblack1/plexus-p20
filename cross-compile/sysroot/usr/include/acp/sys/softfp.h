/* SID */
/* @(#)softfp.h	5.1 4/22/86 */

/************************************************
 *						*
 *	MC68000 IEEE Floating Point Package	*
 *						*
 *	Kevin Enderby 				*
 *	1852 Edgewood Dr.			*
 *	Palo Alto, CA 94303			*
 *	(415) 321-1154				*
 *						*
 *	Release 2.0		6/15/82		*
 ************************************************/

/* masks to decode the floating point opcodes (16 bit masks) */
TYPEMASK	= 0x01c0	|type field of fp instruction
OPCLSMASK	= 0xe000	|opcode class field of fp instruction
EXTNMASK	= 0x007f	|extension field of fp instruction 
CONDMASK	= 0x000f	|condition field of fp branch instructions
DSTFPNMASK	= 0x0380	|fpn destination field of fp instruction
SRCFPNMASK	= 0x1c00	|fpn source field of fp instruction
DTYPEMASK	= 0x1c00	|data type field of fp instruction
EAMODEMASK	= 0x0038	|mode field of a 68000 <ea>
REGMASK		= 0x0007	|register field of a 68000 <ea>

/* masks and bit possitions to decode the extension word for indexed modes */
IREGMASK	= 0x7000	|index register field of the extension word
M6DABIT		= 15		|data or address register bit (mode 6)
M6WLBIT		= 11		|word or long bit (mode 6)

/* floating point operation extension field encoding */
MOVE		= 0x0000	|move
ADD		= 0x0001	|addtion
SUB		= 0x0002	|subtraction
MULT		= 0x0003	|multiply
DIV		= 0x0004	|division
INTPO		= 0x000b	|interger part of
ABS		= 0x000c	|absolute value
NEG		= 0x000e	|negation

/* special floating point register src and dest fields encoding */
Cntrl		= 0x0800	|fp control register
Status		= 0x1000	|fp status register
Ccr		= 0x1800	|fp condition code register

/* masks used by integer part of to clear fractional bits */
FINTHMASK	= 0xfff00000	|used to clear fractional bits in the first 20
				| bits of the mantissa
FINTLMASK	= 0xffffffff	|used to clear fractional bits in the last 32
				| bits of the mantissa
 
/* special value exponents */
NANEXPDB	= 0x7ff0	|not a number exponent for doubles
NANEXPFL	= 0x7f80	|not a number exponent for floats
INFEXPDB	= 0x7ff0	|infinity exponent for doubles (short)
INFEXPDBL	= 0x7ff00000	|infinity exponent for doubles (long)
INFEXPFL	= 0x7f80	|infinity exponent for floats

/* condition codes masks */
FEQ		= 0x0000	|condition code for equal
FLT		= 0x0001	|condition code for less than
FGT		= 0x0002	|condition code for greater than
FUNOR		= 0x0003	|condition code for unordered

/* trap byte bit positions */
INVOP		= 0		|invalid operation
OVFLOW		= 1		|overflow
UNFLOW		= 2		|underflow
DIVZERO		= 3		|divide by zero
INTOVFL		= 5		|integer overflow
RESEXP		= 6		|reserved exponent seen on input
VALIDTRAPS	= 0x6e		|mask of valid trap bits in the trap byte

/* offsets from u.u_fp after a5 has been loaded with the address of u.u_fp */
/* fp trap byte */
#define TRAP	a5@(1)
/* fp status word */
#define STATUS	a5@
/* fp enable byte */
#define ENABLE	a5@(3)
/* fp control word */
#define CNTRL	a5@(2)
/* fp condition code word */
#define CCR	a5@(4)

/* offsets from u.u_ar0 after a6 has been loaded with the address of u.u_ar0 */
/* user's pc */
#define UPC	a6@(72)

/* masks to put together and take apart floating point numbers */
MBITSDB		= 52		|number of mantissa bits in a double
MBITSFL		= 23		|number of mantissa bits in a float
EXPDB		= 0x7ff0	|mask for double exponent
EXPFL		= 0x7f80	|mask for float exponent
MANDB		= 0x000f	|mask for first word of mantissa (short)
MANDBL		= 0x000fffff 	|mask for first word of mantissa (long)
MANFLL		= 0x007fffff	|mask for float mantissa (long)
IMP1DB		= 20		|implied 1 bit possition for a double
IMP1FL		= 23		|implied 1 bit possition for a float
MAN1DB		= 4		|number of mantissa bits in the first word of a double
MAN1FL		= 7		|number of mantissa bits in the first word of a float
XCESDB		= 1023		|exponent bias for a double
XCESFL		= 127		|exponent bias for a float
MAXXDB		= 1023		|maximum exponent for a double
MAXXFL		= 127		|maximum exponent for a float
MINXDB		= -1022		|miminum exponent for a double
MINXFL		= -126		|miminum exponent for a float

SIGNBIT		= 0x8000	|sign bit mask (short)

/* rounding masks */
STKBIT		= 29		|skicky bit possition
STKMASK		= 0x3fffffff	|mask to know if sticky bit is to be set
STKSET		= 0x3fffffff	|mask to set sticky bit
STKCLR		= 0xc0000000	|mask to clear sticky bit
GUARDBIT	= 0x80000000	|the guard bit in rounding

/* masks for normalization */
DSHDB		= 0xfff00000	|mask to decide which direction to shift
RRMASKDB	= 0xffe00000	|mask to know when to stop rolling right for a double

/* rounding mode masks */
MODEMASK	= 0x03		|mask for the mode bits in the status word
RN		= 0x00		|round to nearest
RZ		= 0x01		|round to zero
RP		= 0x02		|round to + infinity
RM		= 0x03		|round to - infinity
