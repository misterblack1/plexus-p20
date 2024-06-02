// C library -- toupper
//
//	This routine is a fast machine language version of the
//	following C routine:
//
//		int toupper (c)
//			register int c;
//		{
//			if (c >= 'a' && c <= 'z')
//				c += 'A' - 'a';
//			return c;
//		}
//
.globl	_toupper

_toupper:
ld	r7,2(r15)	// pick up the argument
cpb	rl7,$'a'	// Is it too low?
jr	ult,t0		//  ...yes
cpb	rl7,$'z'	// Too high?
jr	ugt,t0		// ...yes
addb	rl7,$('A'-'a')	// Make it upper case
t0:
ret			// Return
