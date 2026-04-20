/*  */
/* SID @(#)attch.h	5.1 */
/* @(#)attch.h	1.3 5/1/83 */




	/*-----------------------------------------------------------*\
	| attach messages. These are the formates of the attach queue |
	| messages as seen by the external process. The data from a   |
	| cb is copied into the buffer.				      |
	\*-----------------------------------------------------------*/

	/* user: connection open request message */

struct ucorm {
	ushort	    uco_type;		/* type of attach queue message */
	struct vc **uco_vcaddr;		/* local connection address for open */
	ushort	    uco_acknak;		/* ack/nak space(can use same struct)*/
	ushort	    uco_scatter;	/* scatter mode on read/write */
	char        uco_name[14];	/* name of requester */
	char	    uco_host[14];	/* host name */
	ushort	    uco_length;		/* number of characters of data */
	char	    uco_data[2];	/* foreign supplied data */
};

	/* user: connection open acknowledgement message */

struct ucoam {
	ushort	    uca_type;		/* type of attach queue message */
	struct vc **uca_vcaddr;		/* local connection address for open */
	ushort	    uca_status;		/* status info concerning open reply */
};

	/* user: connection closed message */


struct uccm {
	ushort	    ucc_type;		/* type of attach queue message */
	struct vc **ucc_vcaddr;		/* local connection address for close*/
};

	/* user: open error message */

struct uoem {
	ushort	    uoe_type;		/* type of attach queue message */
	struct vc **uoe_vcaddr;		/* local connection address for error*/
	ushort	    uoe_status;		/* status info concerning error*/
	char	    uoe_name[14];	/* originator name */
	char	    uoe_host[14];	/* originator's host */
};

	/* user: connection error message */

struct ucem {
	ushort	    uce_type;		/* type of attach queue message */
	struct vc **uce_vcaddr;		/* local connection address for error*/
	ushort	    uce_status;		/* status info concerning error*/
};

#define CONN_OPEN   1
#define CONN_RPLY   2
#define CONN_CLOSED 3
#define OPEN_ERR    4
#define CONN_ERR    9
