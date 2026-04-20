/*  */
/* SID @(#)msgs.h	5.1 */
/* @(#)msgs.h	1.4 6/9/83 */



	/*-------------------------------------------------------*\
	| message structures. These are contained within the data |
	| field of the communication block (struct cb).		  |
	\*-------------------------------------------------------*/


	/* open request message format (r - request) */

struct oncfr {
	union {
		struct {
			unsigned char nor_a;
			unsigned char nor_c;
		} nor_b;
		ushort nor_d;
	} nor_e;
	ushort	      nor_status;	/* status of open request foreign use*/
	char	      nor_mynm[14];	/* name of source process */
	char          nor_name[14];	/* name of destination open port */
};

#define nor_newcon nor_e.nor_d
#define nor_atch   nor_e.nor_b.nor_a
#define nor_conn   nor_e.nor_b.nor_c


	/* open request reply message format (a - acknowledgement) */

struct oncfa {
	union {
		struct {
			unsigned char noa_a;
			unsigned char noa_c;
		} noa_b;
		ushort noa_d;
	} noa_e;
	ushort	      noa_status;	/* return status of open request */
	ushort	      noa_fconn;	/* foreign connection sequence number*/
};

#define noa_src  noa_e.noa_d
#define noa_atch noa_e.noa_b.noa_a
#define noa_conn noa_e.noa_b.noa_c

	/* defines for message types */

#define NULL_RQST 0
#define OPEN_RQST 1
#define OPEN_RPLY 2
#define WRIT_RQST 3
#define CLOS_RQST 4
#define  ERR_RQST 5
#define RUTH_RQST 6
