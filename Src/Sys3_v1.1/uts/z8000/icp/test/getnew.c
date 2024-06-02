#include <termio.h>
#include <sys/ioctl.h>
getnew(fd)
int fd;
{
    struct termio val;

    ioctl(fd,TCGETA,&val);

		/* input modes */
    printf("input modes: %o\n",val.c_iflag);

    if(val.c_iflag & IGNBRK)
	printf("\tIGNBRK - Ignore break condition\n");
    if(val.c_iflag & BRKINT)
	printf("\tBRKINT - Signal interrupt on break\n");
    if(val.c_iflag & IGNPAR)
	printf("\tIGNPAR - Ignore characters with parity errors\n");
    if(val.c_iflag & PARMRK)
	printf("\tPARMRK - Mark parity errors\n");
    if(val.c_iflag & INPCK)
	printf("\tINPCK  - Enable input parity checking\n");
    if(val.c_iflag & ISTRIP)
	printf("\tISTRIP - Strip character (8bits -> 7bits)\n");
    if(val.c_iflag & INLCR)
	printf("\tINLCR  - Map NL to CR on input\n");
    if(val.c_iflag & IGNCR)
	printf("\tIGNCR  - Ignore CR\n");
    if(val.c_iflag & ICRNL)
	printf("\tICRNL  - Map CR to NL on input\n");
    if(val.c_iflag & IUCLC)
	printf("\tIUCLC  - Map upper-case to lower-case on input\n");
    if(val.c_iflag & IXON)
	printf("\tIXON   - Enable start/stop output control\n");
    if(val.c_iflag & IXANY)
	printf("\tIXANY  - Enable any character to restart output\n");
    if(val.c_iflag & IXOFF)
	printf("\tIXOFF  - Enable start/stop input control\n");

		/* output modes */
    printf("output modes: %o\n",val.c_oflag);

    if(val.c_oflag & OPOST)
	printf("\tOPOST  - Postprocess output\n");
    if(val.c_oflag & OLCUC)
	printf("\tOLCUC  - Map lower case to upper on output\n");
    if(val.c_oflag & ONLCR)
	printf("\tONLCR  - Map NL to CR-NL on output\n");
    if(val.c_oflag & OCRNL)
	printf("\tOCRNL  - Map CR to NL on output\n");
    if(val.c_oflag & ONOCR)
	printf("\tONOCR  - No CR output at column 0\n");
    if(val.c_oflag & ONLRET)
	printf("\tONLRET - NL performs CR function\n");
    if(val.c_oflag & OFILL)
	printf("\tOFILL  - Use fill characters for delay\n");
    if(val.c_oflag & OFDEL)
	printf("\tOFDEL  - Fill is DEL, else NULL\n");
    if((val.c_oflag&NLDLY) == NL0)
	printf("\tNL0    - new-line delay (0)\n");
    if((val.c_oflag&NLDLY) == NL1)
	printf("\tNL1    - new-line delay (~.1 sec)\n");
    if((val.c_oflag&CRDLY) == CR0)
	printf("\tCR0    - carriage-return delay (0)\n");
    if((val.c_oflag&CRDLY) == CR1)
	printf("\tCR1    - carriage-return delay (var.)\n");
    if((val.c_oflag&CRDLY) == CR2)
	printf("\tCR2    - carriage-return delay (.1 sec)\n");
    if((val.c_oflag&CRDLY) == CR3)
	printf("\tCR3    - carriage-return delay (.15 sec)\n");
    if((val.c_oflag&TABDLY) == TAB0)
	printf("\tTAB0   - horizontal-tab delay (0)\n");
    if((val.c_oflag&TABDLY) == TAB1)
	printf("\tTAB1   - horizontal-tab delay (var.)\n");
    if((val.c_oflag&TABDLY) == TAB2)
	printf("\tTAB2   - horizontal-tab delay (.1 sec)\n");
    if((val.c_oflag&TABDLY) == TAB3)
	printf("\tTAB3   - Expand tabs to spaces\n");
    if((val.c_oflag&BSDLY) == BS0)
	printf("\tBS0    - backspace delays (0)\n");
    if((val.c_oflag&BSDLY) == BS1)
	printf("\tBS1    - backspace delays (.05 sec)\n");
    if((val.c_oflag&VTDLY) == VT0)
	printf("\tVT0    - vertical-tab delay (0)\n");
    if((val.c_oflag&VTDLY) == VT1)
	printf("\tVT1    - vertical-tab delay (?)\n");
    if((val.c_oflag&FFDLY) == FF0)
	printf("\tFF0    - form-feed delays (0)\n");
    if((val.c_oflag&FFDLY) == FF1)
	printf("\tFF1    - form-feed delays (?)\n");

		/* control modes */
    printf("control modes: %o\n",val.c_cflag);

    if((val.c_cflag&CBAUD) == B0)
	printf("\tB0     - Hang up\n");
    if((val.c_cflag&CBAUD) == B50)
	printf("\tB50    - 50 Baud\n");
    if((val.c_cflag&CBAUD) == B75)
	printf("\tB75    - 75 Baud\n");
    if((val.c_cflag&CBAUD) == B110)
	printf("\tB110   - 110 Baud\n");
    if((val.c_cflag&CBAUD) == B134)
	printf("\tB134   - 134.5 Baud\n");
    if((val.c_cflag&CBAUD) == B150)
	printf("\tB150   - 150 Baud\n");
    if((val.c_cflag&CBAUD) == B200)
	printf("\tB200   - 200 Baud\n");
    if((val.c_cflag&CBAUD) == B300)
	printf("\tB300   - 300 Baud\n");
    if((val.c_cflag&CBAUD) == B600)
	printf("\tB600   - 600 Baud\n");
    if((val.c_cflag&CBAUD) == B1200)
	printf("\tB1200  - 1200 Baud\n");
    if((val.c_cflag&CBAUD) == B1800)
	printf("\tB1800  - 1800 Baud\n");
    if((val.c_cflag&CBAUD) == B2400)
	printf("\tB2400  - 2400 Baud\n");
    if((val.c_cflag&CBAUD) == B4800)
	printf("\tB4800  - 4800 Baud\n");
    if((val.c_cflag&CBAUD) == B9600)
	printf("\tB9600  - 9600 Baud\n");
    if((val.c_cflag&CBAUD) == EXTA)
	printf("\tEXTA   - External A\n");
    if((val.c_cflag&CBAUD) == EXTB)
	printf("\tEXTB   - External B\n");
    if((val.c_cflag&CSIZE) == CS5)
	printf("\tCS5    - 5 bits/char\n");
    if((val.c_cflag&CSIZE) == CS6)
	printf("\tCS6    - 6 bits/char\n");
    if((val.c_cflag&CSIZE) == CS7)
	printf("\tCS7    - 7 bits/char\n");
    if((val.c_cflag&CSIZE) == CS8)
	printf("\tCS8    - 8 bits/char\n");
    if(val.c_cflag & CSTOPB)
	printf("\tCSTOPB - Send two stop bits\n");
    else
	printf("\t\b-CSTOPB - Send one stop bit\n");
    if(val.c_cflag & CREAD)
	printf("\tCREAD  - Enable receiver\n");
    if(val.c_cflag & PARENB)
	printf("\tPARENB - Parity enable\n");
    if(val.c_cflag & PARODD)
	printf("\tPARODD - Odd parity\n");
    else
	printf("\t\b-PARODD - Even parity\n");
    if(val.c_cflag & HUPCL)
	printf("\tHUPCL  - Hang up on last close\n");
    if(val.c_cflag & CLOCAL)
	printf("\tCLOCAL - Local line\n");
    else
	printf("\t\b-CLOCAL - Dial-up line\n");

		/* line discipline 0 modes */
    printf("line discpline 0 mode: %o\n",val.c_lflag);

    if(val.c_lflag & ISIG)
	printf("\tISIG   - Enable signals\n");
    if(val.c_lflag & ICANON)
	printf("\tICANON - Canonical input (erase and kill processing)\n");
    if(val.c_lflag & XCASE)
	printf("\tXCASE  - Canonical upper/lower presentation\n");
    if(val.c_lflag & ECHO)
	printf("\tECHO   - Enable echo\n");
    if(val.c_lflag & ECHOE)
	printf("\tECHOE  - Echo erase character as BS-SP-BS\n");
    if(val.c_lflag & ECHOK)
	printf("\tECHOK  - Echo NL after kill character\n");
    if(val.c_lflag & ECHONL)
	printf("\tECHONL - Echo NL\n");
    if(val.c_lflag & NOFLSH)
	printf("\tNOFLSH - Disable flush after interrupt or quit\n");


		/* line discpline */

    printf("line discpline: %o\n",val.c_line);

		/* control characters */

    printf("control characters\n");
    printf("\t%o\tinterrupt\n",val.c_cc[0]);
    printf("\t%o\tquit\n",val.c_cc[1]);
    printf("\t%o\terase\n",val.c_cc[2]);
    printf("\t%o\tkill\n",val.c_cc[3]);
    printf("\t%o\tend-of-file\n",val.c_cc[4]);
    printf("\t%o\tline delimiter\n",val.c_cc[5]);
    printf("\t%o\treserved\n",val.c_cc[6]);
    printf("\t%o\treserved\n",val.c_cc[7]);
}
