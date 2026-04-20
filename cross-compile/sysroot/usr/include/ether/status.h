/*  */
/* SID @(#)status.h	5.1 */
/* @(#)status.h	1.3 5/1/83 */




	/* INTERLAN Ethernet Command Status Codes */

#define SUCCESS   0x0		/* successful completion */

#define RETRYGOOD 0x1		/* successful with retries */

#define BAD_CMND  0x2		/* Illegal Command */

#define INAP_CMND 0x3		/* Inappropriate Command */

#define FAILURE   0x4		/* Failure */

#define BUFOVFLOW 0x5		/* Buffer size exceeded */

#define FRM2SMALL 0x6		/* Frame too small */

#define COLLISION 0x8		/* Excessive Collisions */

#define BUF_ALIGN 0xa		/* Buffer Alignment Error */


		/* INTERLAN Ethernet Diagnostic Status Codes */

#define MEM_ERR   0x1		/* NM10 Memory Error */

#define DMA_ERR   0x2		/* NM10 DMA Error */

#define XMIT_ERR  0x3		/* Transmitter Error */

#define REC_ERR   0x4		/* Receiver Error */

#define LOOP_ERR  0x5		/* Loopback Error */




		/* INTERLAN Ethernet frame status byte (on read) */

#define CRC_ERR		0x01	/* CRC error (pg. 21) */

#define ALLIGN_ERR	0x02	/* alignment error (pg. 21) */
