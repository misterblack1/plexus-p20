/* SID @(#)command.h	5.1 */
/*  */
/* @(#)command.h	1.3 5/1/83 */




#ifdef RESET
#undef RESET
#endif
		/* INTERLAN Ethernet Control Register Commands */

#define MOD_LOOP    0x01	/* Set Module Interface Loopback (see pg 38) */

#define INT_LOOP    0x02	/* Set Internal Loopback (see pg 40) */

#define CLR_LOOP    0x03	/* Clear Loopback (see pg 42) */

#define SET_PROM    0x04	/* Set Promiscuous Mode (see pg 43) */

#define CLR_PROM    0x05	/* Clear Promiscuous Mode (see pg 44) */

#define REC_ON_ERR  0x06	/* Set Receive-on-Error Mode (see pg 45) */

#define CLR_REC_ERR 0x07	/* Clear Receive-on-Error Mode (see pg 46) */

#define OFFLINE     0x08	/* Go Offline (see pg 47) */

#define ONLINE      0x09	/* Go Online (see pg 48) */

#define SELFTEST    0x0a	/* Run Onboard Diagnostics (see pg 49) */

#define STATISTICS  0x18	/* Report and Reset Statistics (see pg 52) */

#define DELAY_TIME  0x19	/* Report Collision Delay Times (see pg 56) */

#define LD_DATA     0x28	/* Load Transmit Data (see pg 60) */

#define LDSND_DATA  0x29	/* Load Transmit Data and Send (see pg 61) */

#define SET_ADDRSS  0x2a	/* Load Group Address(es) (see pg 63) */

#define CLR_ADDRSS  0x2b	/* Delete Group Address(es) (see pg 65) */

#define RESET       0x3f	/* Reset (see pg 67) */
