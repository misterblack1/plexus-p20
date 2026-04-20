/* SID */
/* @(#)clkstruct.h	5.1 4/22/86 */

/* definition of system configuration information held in 
   clock/calendar ram.  This information is used mostly
   for diagnostic purposes
*/

#ifndef ASSM
struct config {
	unsigned short console;	/* points to console port */
	unsigned short baud[8];
	unsigned void1:8;unsigned notused:7 ; unsigned autoboot:1;
	unsigned void2:8; unsigned devsize:2; unsigned memsize:6;
	unsigned short rev[4];
	unsigned void3:8; unsigned icp:4; unsigned protocol:4;
	unsigned void3a:8; unsigned acp:8;
	unsigned void4:8; unsigned excelan:8;
	unsigned voaid4a:8; unsigned clockspeed:8;
	struct s {
		unsigned void5:8; unsigned controller:8;
		unsigned void6:8; unsigned luns:8;
		unsigned void7:8; unsigned luntype:8;
		} scsi[8];
	unsigned short word43;
	unsigned short word44;
	unsigned short word45;
	unsigned short word46;
	unsigned short word47;
	unsigned short word48;
	unsigned short checksum;
};
#endif

#define CONFIGCHECKSUM	0x5a /* sum of all words must be 0x5a */
#define CLOCKCOUNT	50 /* number of entries (high words are undefined */
#define CONFIGSTART	CALREGD + 1 /* start of configuration info */
#define CLOCKCONSOLE	CALREGD + 1	
#define BAUDSTART	CLOCKCONSOLE + 2
#define LUNTYPE(P,SCSIDEV,LUN) (( P ->scsi[SCSIDEV].luntype>>(LUN*2))&3)
#define LUNTHERE(P,SCSIDEV,LUN) (( P ->scsi[SCSIDEV].luns>>LUN)&1)
#define BRDTHERE(P,SCSIDEV) ((P->scsi[SCSIDEV].controller))
#define HARD 0	 /* hard disk entry in scsi[].luns (2 bit field) */
#define TAPE 1	 /* tape entry in scsi[].luns (2 bit field) */
#define FLOPPY 2 /* floppy disk entry in scsi[].luns (2 bit field) */
#define NOTEST 10 /* if protocol set to this selftest not run in pod */
