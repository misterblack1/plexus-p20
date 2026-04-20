/*  */
/* SID @(#)frames.h	5.1 */
/* @(#)frames.h	1.3 5/1/83 */



	/*-----------------*\
	| pdlc frame types. |
	\*-----------------*/

	/* numbered information frame */

struct iframe {
	i_addr;
	i_ctrl;
};

	/* numbered supervisory frame */

struct sframe {
	s_addr;
	s_ctrl;
};

	/* unnumbered control frame */

struct uframe {
	u_addr;
	u_ctrl;
};

	/* Address types */

#define A	1
#define B	3

	/* iframe fields */

#define NR	0340		/* transmitter receive sequence number */
#define PF	0020		/* poll/final bit */
#define NS	0016		/* transmitter send sequence number */
#define ITYPE	0001		/* bit 0 == 0 indicates I frame */

	/* sframe fields */

#define S	0014		/* supervisory function bits */
#define STYPE	0003		/* bits 0,1 == (01) indicate S frame */

	/* uframe fields */

#define M2	0340		/* modifier function bits - high field */
#define M1	0014		/* modifier function bits - low  field */
#define UTYPE	0003		/* bits 0,1 == (11) indicate U frame */
