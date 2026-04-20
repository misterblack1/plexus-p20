/*  */
/* @(#)prof.h	5.1 4/22/86 */

/*	@(#)prof.h	1.3	*/
#ifndef MARK
#define MARK(L)	{}
#else
#undef MARK
#ifdef vax
#define MARK(L)	{\
		asm("	.data");\
		asm("	.align	4");\
		asm(".L.:");\
		asm("	.long	0");\
		asm("	.text");\
		asm("M.L:");\
		asm("	nop;nop");\
		asm("	movab	.L.,r0");\
		asm("	jsb	mcount");\
		}
#endif
#ifdef m68
#define MARK(L)	{\
		asm("	.data");\
		asm("	.align	2");\
		asm(".L.:");\
		asm("	.long	0");\
		asm("	.text");\
		asm("M.L:");\
		asm("	movl	#.L.,d0");\
		asm("	jsr	mcount");\
		}
#endif
#ifdef u3b
#define MARK(L)	{\
		asm("	.data");\
		asm("	.align	4");\
		asm(".L.:");\
		asm("	.word	0");\
		asm("	.text");\
		asm("M.L:");\
		asm("	movw	&.L.,%r0");\
		asm("	jsb	_mcount");\
		}
#endif
#ifdef pdp11
#define MARK(L)	{\
		asm("	.bss");\
		asm(".L.:");\
		asm("	.=.+2");\
		asm("	.text");\
		asm("M.L:");\
		asm("	mov	$.L.,r0");\
		asm("	jsr	pc,mcount");\
		}
#endif
#endif
