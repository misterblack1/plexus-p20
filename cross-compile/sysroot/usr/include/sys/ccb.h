/*   @(#)ccb.h	5.2 */
/* ccb.h */

/* This file contains the definitions for the Common Circuits Board. */

/* ccb port defines */
/* Note that DC_BAD is not valid in ccb register one.
   And UPS_2 is not valid in ccb register zero.
*/
#define	CCB_INT		0x01	/* ccb interrupt line active */
#define	CCB_TEMP_1	0x02	/* temp 1 interrupt */
#define CCB_TEMP_2	0x04	/* temp 2 interrupt */
#define CCB_SWINT	0x08	/* software interrupt active */
#define CCB_KEYOFF	0x10	/* key off interrupt */
#define CCB_UPS_1	0x20	/* ups interrupt */
#define CCB_UPS_2	0x40	/* ups two value */
#define CCB_DC_BAD	0x80	/* power fail interrupt */

/* ccb ioctl commands */

/* pirate and kicker only */
#define	C_C_POWER_DOWN	1	/* power down system */
#define	C_C_OFF		2	/* turn off 'keepon' */
#define	C_C_STAT	3	/* put status in arg */

/* Pirate/kicker/p60/p35 commands */
#define	C_C_SAFE	4	/* turn on safe mode */
#define	C_C_NO_SAFE	5	/* turn off safe mode */
#define	C_C_SWITCHES	6	/* put cpu board switch settings in arg */
#define	C_C_RESET	7	/* reset the system */
#define	C_C_INITMODE	8	/* put initmode in arg 1-7,10(single-user) */
#define	C_C_DEBUG	9	/* call sysdebug */
