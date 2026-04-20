/* SID @(#)crtctl.h	5.1 */
/* @(#)crtctl.h	6.1 */

/*
	Define the cursor control codes
*/
#define ESC	033	/* Escape for command */

/* Commands */
#define CUP	0101	/* Cursor up */
#define CDN	0102	/* Cursor down */
#define CRI	0103	/* Cursor right */
#define CLE	0104	/* Cursor left */
#define NL	0134	/* Terminal newline function */
#define HOME	0105	/* Cursor home */
#define VHOME	0106	/* cursor home to variable portion */
#define LCA	0107	/* Load cursor, followed by (x,y) in (col,row) */
#define CRTN	0133	/* Return cursor to beginning of line */

#define STB	0110	/* Start blink */
#define SPB	0111	/* Stop blink */
#define CS	0112	/* Clear Screen */
#define CM	0135	/* Clear Memory */
#define EEOL	0113	/* Erase to end of line */
#define EEOP	0114	/* Erase to end of page */
#define DC	0115	/* Delete character */
#define DL	0116	/* Delete line */
#define IC	0117	/* Insert character */
#define IL	0120	/* Insert line */
#define KBL	0121	/* keyboard lock */
#define KBU	0122	/* keyboard unlock */
#define ATAB	0123	/* Set column of tabs */
#define STAB	0124	/* Set single tab */
#define CTAB	0125	/* Clear Tabs */
#define USCRL	0126	/* Scroll up one line */
#define DSCRL	0127	/* Scroll down one line */
#define ASEG	0130	/* Advance segment */
#define BPRT	0131	/* Begin protect */
#define EPRT	0132	/* End   protect */

#define SVSCN	0136	/* Define variable portion of screen (OS only) */
#define UVSCN	0137	/* Scroll Up variable portion of screen */
#define DVSCN	0140	/* Scroll Down variable portion of screen */

#define SVID	0141	/* Set Video Attributes */
#define CVID	0142	/* Clear Video Attributes */
#define DVID	0143	/* Define Video Attributes */
/* Video Attribute Definitions */
#define VID_NORM	000	/* normal */
#define VID_UL		001	/* underline */
#define VID_BLNK	002	/* blink */
#define VID_REV	004	/* reverse video */
#define VID_DIM	010	/* dim intensity */
#define VID_BOLD	020	/* bright intensity */
#define VID_OFF	040	/* blank out field */

#define BRK	000	/* transmit break */
#define HIQ	001	/* Put remainder of write on high priority queue. */
