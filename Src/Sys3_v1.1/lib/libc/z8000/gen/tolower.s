// C library -- tolower
//
//	This routine is a fast machine language version of the
//	following C routine:
//
//		int tolower (c)
//			register int c;
//		{
//			if (c >= 'A' && c <= 'Z')
//				c += 'a' - 'A';
//			return c;
//		}
//
.globl	_tolower

_tolower:
ld	r7,2(r15)	// pick up the argument
cpb	rl7,$'A'	// Is it too low?
jr	ult,t0		//  ...yes
cpb	rl7,$'Z'	// Too high?
jr	ugt,t0		// ...yes
addb	rl7,$('a'-'A')	// Make it lower case
t0:
ret			// Return
