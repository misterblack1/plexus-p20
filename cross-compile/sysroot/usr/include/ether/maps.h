/*  */
/* SID @(#)maps.h	5.1 */
/* @(#)maps.h	1.6 5/14/84 */



	/* mappings and communication tables */

	/*-----------------------------------------------*\
	| map plexus # to ethernet address. This table is |
	| initialized by etinit()			  |
	\*-----------------------------------------------*/

struct ptoet {
	unsigned short  et_type;  /* type of connection: ethernet, etc */
	char et_addr[6];	/* ethernet address */
};

	/*-----------------------------------------------*\
	| map ethernet address to plexus #. This table is |
	| initialized by etinit()			  |
	\*-----------------------------------------------*/

struct ettop {
	char p_addr[6];
	unsigned short  p_host;
};
