/*
 * This source contains the trap and interrupt vectors. The object
 * produced by compiling the source must be the first object loaded
 * to form the system image since the psap must be on a 256 byte
 * boundary and the boot starts unix at text 0.
 */

#include "sys/plexus.h"

/* these bits must match the defs in pd.c and pt.c */
#define DISKBIT 1
#define TAPEBIT 2

/* externals */
.globl	start 
.globl	psa
.globl	_utsname
.globl	t_ei
.globl	t_pi
.globl	t_sc
.globl	t_seg
.globl	t_nmi
.globl	t_nvi
.globl	i_bad
.globl	i_mbad
.globl	i_sbad
.globl	i_clk
.globl	i_busto
.globl	i_sstep
.globl	i_mrerr
.globl	i_lcltty
.globl	_debug


psa:
jp	(LONGSEGADX | SDSEG)	/* jump to start code, note that this is done */
.word	start			/* in segmented mode thus the address fake */
jp	dump			/* code that links to dump is at location 6 */
				/* this also uses the reserved word from the */
				/* extended instruction trap */

		/* extended inst trap */
.word	B_SYS + B_NVI
.word	SDSEG
.word	t_ei

				/* the reserved word for nvi is used for the */
				/* location of the unix system name  (loc 16) */
.word	_utsname /* priveleged inst trap */
.word	B_SYS + B_NVI
.word	SDSEG
.word	t_pi

.word	0	/* syscall inst trap */
.word	B_SYS + B_NVI
.word	SDSEG
.word	t_sc

.word	0	/* segmentation trap */
.word	B_SYS + B_NVI
.word	SDSEG
.word	t_seg

.word	0	/* non maskable interrupt */
.word	B_SYS + B_NVI
.word	SDSEG
.word	t_nmi

.word	0	/* non vectored interrupt */
.word	B_SYS
.word	SDSEG
.word	t_nvi

.word	0	/* vectored interrupts */
.word	B_SYS + B_NVI

.word	SDSEG	/* vectored interrupt id 0, master 8259 ir0 */
.word	i_mbad
.word	SDSEG	/* vectored interrupt id 2 */
.word	i_busto
.word	SDSEG	/* vectored interrupt id 4 */
.word	i_clk
.word	SDSEG	/* vectored interrupt id 6 */
.word	i_mrerr
.word	SDSEG	/* vectored interrupt id 8 */
.word	i_mbad
.word	SDSEG	/* vectored interrupt id 10 */
.word	i_sstep
.word	SDSEG	/* vectored interrupt id 12 */
.word	i_lcltty
.word	SDSEG	/* vectored interrupt id 14 */
.word	i_mbad

.word	SDSEG	/* vectored interrupt id 16, slave 8259 ir0 */
.word	i_mb0	/* icp 4 or interlan ethernet board */
.word	SDSEG	/* vectored interrupt id 18 */
.word	i_mb1
.word	SDSEG	/* vectored interrupt id 20 */
.word	i_mb2	/* ICP 1 */
.word	SDSEG	/* vectored interrupt id 22 */
.word	i_mb3	/* ICP 0 */
.word	SDSEG	/* vectored interrupt id 24 */
.word	i_mb4	/* ICP 2 */
.word	SDSEG	/* vectored interrupt id 26 */
.word	i_mb5
.word	SDSEG	/* vectored interrupt id 28 */
.word	i_mb6	/* ICP 3 */
.word	SDSEG	/* vectored interrupt id 30 */
/* .word	i_mb7	 ICP 4 */
.word	i_mb7	/* IMSC */
.word	SDSEG	/* vectored interrupt id 32, first unused vector */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 34 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 36 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 38 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 40 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 42 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 44 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 46 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 48 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 50 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 52 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 54 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 56 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 58 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 60 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 62 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 64 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 66 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 68 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 70 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 72 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 74 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 76 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 78 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 80 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 82 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 84 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 86 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 88 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 90 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 92 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 94 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 96 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 98 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 100 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 102 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 104 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 106 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 108 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 110 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 112 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 114 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 116 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 118 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 120 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 122 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 124 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 126 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 128 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 130 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 132 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 134 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 136 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 138 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 140 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 142 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 144 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 146 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 148 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 150 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 152 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 154 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 156 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 158 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 160 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 162 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 164 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 166 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 168 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 170 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 172 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 174 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 176 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 178 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 180 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 182 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 184 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 186 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 188 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 190 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 192 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 194 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 196 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 198 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 200 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 202 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 204 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 206 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 208 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 210 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 212 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 214 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 216 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 218 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 220 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 222 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 224 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 226 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 228 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 230 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 232 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 234 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 236 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 238 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 240 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 242 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 244 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 246 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 248 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 250 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 252 */
.word	i_bad
.word	SDSEG	/* vectored interrupt id 254 */
.word	i_bad

jp	_debug	/* if we somehow get into low core, bail out to sysdebug */			/* the program status area executes and ends up here */

dump:
jp	_debug	/* what we really want to do here is dump memory to disk */
		/* or tape, but for now we will go to debug */

/*
 * handlers for multibus interrupts
 */

/* externals */
.globl	pushregs
.globl	resetslave
.globl	intret
#ifndef VPMSYS
.globl	_isintr
.globl	_rmintr
.globl	_pdintr
#endif
#ifdef VPMSYS
#ifdef VPMSYSIS
.globl	_isintr
.globl	_rmintr
#else
.globl	_pdintr
#endif
#endif
.globl	_siint0
.globl	_siint1
.globl	_siint2
.globl	_siint3
.globl	_siint4
#ifdef ETHER
.globl	_etintr
#endif


i_mb0:
call	pushregs
#ifdef ETHER
call	_etintr
#else
call	_siint4
#endif
call	resetslave
jp	intret

i_mb1:
call	pushregs
#ifndef VPMSYS
call	_rmintr
#endif
#ifdef VPMSYS
#ifdef VPMSYSIS
call	_rmintr
#endif
#endif
call	resetslave
jp	intret

i_mb2:
call	pushregs
call	_siint1
call	resetslave
jp	intret

i_mb3:
call	pushregs
call	_siint0
call 	resetslave
jp	intret

i_mb4:
call	pushregs
call	_siint2
call	resetslave
jp	intret

i_mb5:
call	pushregs
#ifndef VPMSYS
call	_isintr
#endif
#ifdef VPMSYS
#ifdef VPMSYSIS
call	_isintr
#endif
#endif
call	resetslave
jp	intret


i_mb6:
call	pushregs
call	_siint3
call	resetslave
jp	intret

i_mb7:
call	pushregs
#ifndef VPMSYS
ld	r7,_imscint
and	r7,$DISKBIT
jr	z,imb7a
call	_pdintr
imb7a:
ld	r7,_imscint
and	r7,$TAPEBIT
jr	z,imb7b
call	_ptintr
imb7b:
#endif

#ifdef VPMSYS
#ifdef VPMSYSIMSC
ld	r7,_imscint
and	r7,$DISKBIT
jr	z,imb7a
call	_pdintr
imb7a:
ld	r7,_imscint
and	r7,$TAPEBIT
jr	z,imb7b
call	_ptintr
imb7b:
#endif
#endif
call	resetslave
jp	intret
