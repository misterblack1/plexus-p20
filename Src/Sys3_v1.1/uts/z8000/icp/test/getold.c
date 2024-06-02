#include <termio.h>
#include <sys/ttold.h>
#include <sys/ioctl.h>
#define TANDEM	01
#define CBREAK  02
#define O_NL0	00
#define O_NL3	01400
#define O_TAB0  00
#define O_TAB1  02000
#define O_TAB2  04000
#define P_XTABS 06000
#define O_CR0	00
#define O_CR3	030000
#define O_FF0	00
#define O_FF1	040000
#define O_BS0	00
#define O_BS1	0100000
getold(fd)
int fd;
{
    struct sgttyb val;

    ioctl(fd,TIOCGETP,&val);


    printf("terminal control flags: %o\n",val.sg_flags);
    if(val.sg_flags & TANDEM)
	printf("\tTANDEM - input flow control via ^S ^Q\n");
    if(val.sg_flags & CBREAK)
	printf("\tCBREAK - Half-cooked mode\n");
    if(val.sg_flags & O_LCASE)
	printf("\tLCASE  - Map upper case to lower on input\n");
    if(val.sg_flags & O_ECHO)
	printf("\tECHO   - Echo characters (full duplex)\n");
    if(val.sg_flags & O_CRMOD)
	printf("\tCRMOD  - Map CR into LF; echo LF or CR as CR-LF\n");
    if(val.sg_flags & O_RAW)
	printf("\tRAW    - Raw Mode. Wake up on all characters\n");
    if(val.sg_flags & O_ODDP)
	printf("\tODDP   - Odd parity allowed on input\n");
    if(val.sg_flags & O_EVENP)
	printf("\tEVENP  - Even parity allowed on input\n");

/*
 * Delay algorithms
 */
    if((val.sg_flags&0001400) == O_NL0)
	printf("\tNL0    - new-line delay (0)\n");
    if((val.sg_flags&0001400) == O_NL1)
	printf("\tNL1    - new-line delay (var.)\n");
    if((val.sg_flags&0001400) == O_NL2)
	printf("\tNL2    - new-line delay (~.1 sec)\n");
    if((val.sg_flags&0006000) == O_TAB0)
	printf("\tTAB0   - horizontal-tab delay (0)\n");
    if((val.sg_flags&0006000) == O_TAB1)
	printf("\tTAB1   - horizontal-tab delay (var.)\n");
    if((val.sg_flags&0006000) == O_TAB2)
	printf("\tTAB2   - horizontal-tab delay (.1 sec)\n");
    if((val.sg_flags&0006000) == P_XTABS)
	printf("\tTAB3   - Expand tabs to spaces\n");
    if((val.sg_flags&0030000) == O_CR0)
	printf("\tCR0    - carriage-return delay (0)\n");
    if((val.sg_flags&0030000) == O_CR1)
	printf("\tCR1    - carriage-return delay (var.)\n");
    if((val.sg_flags&0030000) == O_CR2)
	printf("\tCR2    - carriage-return delay (.1 sec)\n");
    if((val.sg_flags&0030000) == O_CR3)
	printf("\tCR3    - carriage-return delay (.15 sec)\n");
    if((val.sg_flags&0040000) == O_FF0)
	printf("\tFF0    - form-feed delays (0)\n");
    if((val.sg_flags&0040000) == O_FF1)
	printf("\tFF1    - form-feed delays (?)\n");
    if((val.sg_flags&0100000) == O_BS0)
	printf("\tBS0    - backspace delays (0)\n");
    if((val.sg_flags&0100000) == O_BS1)
	printf("\tBS1    - backspace delays (.05 sec)\n");

/*
 * erase and kill characters.
 */

    printf("\nerase character: %o\n",val.sg_erase);
    printf(  "kill  character: %o\n",val.sg_kill);

/*
 * Input speeds
 */
    printf("\nInput speed: %o\n",val.sg_ispeed);
    if((val.sg_ispeed&017) == B0)
	printf("\tB0     - Hang up\n");
    if((val.sg_ispeed&017) == B50)
	printf("\tB50    - 50 Baud input\n");
    if((val.sg_ispeed&017) == B75)
	printf("\tB75    - 75 Baud input\n");
    if((val.sg_ispeed&017) == B110)
	printf("\tB110   - 110 Baud input\n");
    if((val.sg_ispeed&017) == B134)
	printf("\tB134   - 134.5 Baud input\n");
    if((val.sg_ispeed&017) == B150)
	printf("\tB150   - 150 Baud input\n");
    if((val.sg_ispeed&017) == B200)
	printf("\tB200   - 200 Baud input\n");
    if((val.sg_ispeed&017) == B300)
	printf("\tB300   - 300 Baud input\n");
    if((val.sg_ispeed&017) == B600)
	printf("\tB600   - 600 Baud input\n");
    if((val.sg_ispeed&017) == B1200)
	printf("\tB1200  - 1200 Baud input\n");
    if((val.sg_ispeed&017) == B1800)
	printf("\tB1800  - 1800 Baud input\n");
    if((val.sg_ispeed&017) == B2400)
	printf("\tB2400  - 2400 Baud input\n");
    if((val.sg_ispeed&017) == B4800)
	printf("\tB4800  - 4800 Baud input\n");
    if((val.sg_ispeed&017) == B9600)
	printf("\tB9600  - 9600 Baud input\n");
    if((val.sg_ispeed&017) == EXTA)
	printf("\tEXTA   - External A input\n");
    if((val.sg_ispeed&017) == EXTB)
	printf("\tEXTB   - External B input\n");

/*
 * Output speeds
 */
    printf("\nOutput speed: %o\n",val.sg_ospeed);
    if((val.sg_ospeed&017) == B0)
	printf("\tB0     - Hang up\n");
    if((val.sg_ospeed&017) == B50)
	printf("\tB50    - 50 Baud output\n");
    if((val.sg_ospeed&017) == B75)
	printf("\tB75    - 75 Baud output\n");
    if((val.sg_ospeed&017) == B110)
	printf("\tB110   - 110 Baud output\n");
    if((val.sg_ospeed&017) == B134)
	printf("\tB134   - 134.5 Baud output\n");
    if((val.sg_ospeed&017) == B150)
	printf("\tB150   - 150 Baud output\n");
    if((val.sg_ospeed&017) == B200)
	printf("\tB200   - 200 Baud output\n");
    if((val.sg_ospeed&017) == B300)
	printf("\tB300   - 300 Baud output\n");
    if((val.sg_ospeed&017) == B600)
	printf("\tB600   - 600 Baud output\n");
    if((val.sg_ospeed&017) == B1200)
	printf("\tB1200  - 1200 Baud output\n");
    if((val.sg_ospeed&017) == B1800)
	printf("\tB1800  - 1800 Baud output\n");
    if((val.sg_ospeed&017) == B2400)
	printf("\tB2400  - 2400 Baud output\n");
    if((val.sg_ospeed&017) == B4800)
	printf("\tB4800  - 4800 Baud output\n");
    if((val.sg_ospeed&017) == B9600)
	printf("\tB9600  - 9600 Baud output\n");
    if((val.sg_ospeed&017) == EXTA)
	printf("\tEXTA   - External A output\n");
    if((val.sg_ospeed&017) == EXTB)
	printf("\tEXTB   - External B output\n");
}
