/*  */
/* SID @(#)buffers.h	5.1 */
/* @(#)buffers.h	1.6 8/25/83 */




	/* definition of NCF structures and control blocks */

	/*------------------------------------*\
	| communication block queue descripter |
	\*------------------------------------*/

struct cbque {
	struct cb *cb_head;
	struct cb *cb_tail;
};

	/*-----------------------------------------------------------*\
	| The following is the control structure associated with each |
	| attach. It constists of a control word followed by a vector |
	| of wakeup address.					      |
	\*-----------------------------------------------------------*/

struct attch {
	unsigned int at_mode;	/* attach control mode */
};

	/*---------------------------------------------------------*\
	| The following is a connection descripter. It contains the |
	| information necessary to manage a connection between NCFs |
	\*---------------------------------------------------------*/

struct vc {
    struct vc	 *vc_next;	/* forward pointer or pointer to parent strct*/
    char	  vc_dest;	/* destination host id */
    char 	  vc_state;	/* state of this connection */
    ushort        vc_foreign;	/* foreign connection slot descripter */
    ushort	  vc_lcid;	/* local connection sequence id */
    ushort 	  vc_rcid;	/* remote connection sequence id */
    int 	  vc_toid;	/* timeout id abort address */
    ushort	  vc_type;	/* type of conn (data local/other adr space)*/
    struct cb    *vc_attch;	/* pointer to queued attach message */
    char	  vc_asleep;	/* process sleeping waiting for input*/
    char	  vc_msg;	/* Number of complete messages available */
    struct cbque  vc_read;	/* input read queue */
};

	/*-----------------------------------*\
	| virtual connection queue descripter |
	\*-----------------------------------*/

struct vcque {
	struct vc *vc_head;
	struct vc *vc_tail;
};


	/*-----------------------------------------------------------*\
	| vctab is the control structure for the ncf. It contains the |
	| various paramters necessary to implement the virtual        |
	| connections and to buffer read and write requests.          |
	\*-----------------------------------------------------------*/

struct vctab {
	int          vc_slot;		/* in use flag for attachment */
	ushort 	     vc_nconn;		/* number of virtual connections */
	char         vc_name[14];	/* attached name, null terminated */
	long int     vc_stat;		/* bit vector of waiting input data */
	struct attch vc_parm;		/* attach paramaters */
	struct vc   *vc_conn[N_CONNECT]; /* array of connection descripters */
};


	/*-----------------------------------------------------------*\
	| ntoh is a table that provides a mapping between the various |
	| host names and an internal identfier. Note that ntoh is     |
	| initialized at init time and represents all of the hosts    |
	| known to the local host.				      |
	\*-----------------------------------------------------------*/

struct ntoh {
	char      host_name[N_CPH];
	ushort    host_id;
};
