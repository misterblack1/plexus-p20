/* SID */
/* @(#)acpc.h	5.1 4/22/86 */

/*
 * Global defines which are used to describe the acp 
 * hardware.
 */

/*
 * acp multibus port addresses set into address select jumpers and switches
 */

#define PACP(k) (0xffcf-0x10*k)
#define	PACP0	0xffcf		/* address for acp 0 */
#define	PACP1	0xffbf		/* address for acp 1 */
#define	PACP2	0xffaf		/* address for acp 2 */
#define PACP3	0xff9f		/* address for acp 3 */
#define PACP4	0xff8f		/* address for acp 4 */
