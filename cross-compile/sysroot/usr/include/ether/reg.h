/*  */
/* SID @(#)reg.h	5.1 */
/* @(#)reg.h	1.3 5/1/83 */




		/* INTERLAN Ethernet register definitions */

#define BASEREG 0xff20		/* I/O base register as set by DIP U1 */

#define CMNDREG BASEREG		/* The command register (see pg 23) */
#define C_REG   BASEREG
#define CMNDCODE 0x37		/* mask for command register */

#define STATREG BASEREG+1	/* The status register (see pg 24) */
#define S_REG   BASEREG+1

#define TXDTREG BASEREG+2	/* Transmitter DATA register (see pg 25) */
#define T_DATA  BASEREG+2

#define RECDREG BASEREG+3	/* Receive DATA register (see pg 26) */
#define R_DATA  BASEREG+3

#define INTREG  BASEREG+5	/* Interrupt status register (see pg 27) */
#define IS_REG  BASEREG+5

#define TDE	0x10		/* Transmit Data Empty */
#define RBA	0x8		/* Receive Block Available */
#define RDF	0x4		/* Receive Data Full */
#define SBA	0x2		/* Status Block Available */
#define SRF	0x1		/* Status Register Full */

#define IENBREG BASEREG+8	/* Interrupt enable register (see pg 29) */
#define IE_REG  BASEREG+8
#define INTCODE 0x7		/* Interrupt Code (See pg 29) */

#define EN_RDD	7		/* enable Receive DMA Done interrupt */
#define EN_TDD	6		/* enable Transmit DMA Done interrupt */
#define EN_TDE	5		/* enable Transmit Data Empty interrupt */
#define EN_RBA	4		/* enable Receive Block Available interrupt */
#define EN_RDF	3		/* enable Receive Data Full interrupt */
#define EN_SBA	2		/* enable Status Block Available interrupt */
#define EN_SRF	1		/* enable Status Register Full interrupt */
#define DIABLE	0		/* disable interrupts */
#define CLR_INT	0		/* clear latched interrupt */

#define EBUSREG BASEREG+9	/* Extended bus address register (pg 31) */
#define E_BAR	BASEREG+9

#define HBUSREG BASEREG+10	/* High bus address register (see pg 32) */
#define H_BAR	BASEREG+10

#define LBUSREG BASEREG+11	/* Low Bus address register (see pg 33) */
#define L_BAR   BASEREG+11

#define HCNTREG BASEREG+12	/* High byte count register (see pg 34) */
#define H_BCR	BASEREG+12

#define LCNTREG BASEREG+13	/* Low byte count register (see pg 35) */
#define L_BCR   BASEREG+13
