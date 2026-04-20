/*  */
/* SID @(#)state.h	5.1 */
/* @(#)state.h	1.3 5/1/83 */




	/* states of the ethernet board */

#define INITIAL	0	/* Ethernet board initial state on power up */
#define STARTUP 1	/* Ethernet board going online */
#define SENDING 2	/* Writing data out onto ethernet */
#define RECEIVING 3	/* Receiving data from ethernet */
#define FAIL	4	/* Interlan request has failed */
#define DONE    5	/* Interlan request has completed */
			/* Using OFFLINE ==  0x08 from command.h */
			/* Using ONLINE == 0x09 from command.h */
