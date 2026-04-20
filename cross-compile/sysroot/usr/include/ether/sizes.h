/*  */
/* SID @(#)sizes.h	5.1 */
/* @(#)sizes.h	1.3 5/1/83 */




	/* sizing information for various buffers and tables */

#define HDR	 2	/* space (bytes) for link */

#define HEAD     0	/* extra header required by other devices */

#define RDBOFF   0	/* offset (bytes) to start of rdb substructure */
#define TDBOFF  10	/* offset (bytes) to start of tdb substructure */
#define CBOFF    8	/* offset (bytes) to start of cb substructure */
#define DATOFF  18	/* offset (bytes) to start of data field */
#define CBHDR    6	/* offset (bytes) to start of cb data */

#define CRCSIZ	4	/* # of bytes in crc field */

#define TAIL	0	/* # of bytes following ethernet frame */

#define XMIT_HDR (HDR+HEAD+TDBOFF)/* link header + extra header + tdb offset */
#define PDLCHDR  (HDR+HEAD+DATOFF+CBHDR+TAIL)

#define TBSIZE 512	/* size of transmitted data (bytes) */
#define RBSIZE 512	/* size of received data (bytes) */

#define NPLEXUS 10	/* number of hosts on network */

	/* size of communication buffer */
#define ETSIZE	(sizeof(struct etcom))

	/* following may be moved to different include file later */

#define ETBUFSZ 10000	/* space for dynamic buffers */
#define ETMAPSZ 128
