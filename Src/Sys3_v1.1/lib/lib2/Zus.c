/*
 * CPU USART driver
 */

/****************************************************************/
/*								*/
/* This is a strange driver.  It accepts input from the dnld	*/
/*  program to the USART in the following format:		*/
/*	?,<data in hex ascii>,<checksum>!			*/
/*								*/
/* The purpose of this driver is to enable a download of	*/
/*  UNIX or any other standalone program.  A.out format		*/
/*  object files can be sent over using the -u option of	*/
/*  dnld.  All data is in hex ascii (2 bytes per byte).		*/
/*  The checksum is also in hex ascii, and is a checksum of	*/
/*  all data (not including the seperation characters).		*/
/*								*/
/* The driver starts by ignoring all characters until it	*/
/*  encounters a '?', which must be immediately followed	*/
/*  by a ','.  Any getc() from this device will then return	*/
/*  the character translated from hex ascii.  A ',' or a	*/
/*  '!' signify the end of file.  A (',' means the checksum	*/
/*  is immediately following, '!' while reading data		*/
/*  means something is wrong, and no more reading should be	*/
/*  done.)  Closing the device searches for a ',' if not	*/
/*  already encountered, checks the checksum, and then		*/
/*  waits for a '!' to indicate that the transfer is		*/
/*  complete (which means the tube is probably hooked back up).  */
/*								*/
/****************************************************************/
#include "saio.h"

int us_eof;		/* Flag indicating EOF has been found */
unsigned checksum;	/* Checksum for input, checked on close */
int us_first;		/* Flag indicating we haven't read anything yet */
int dnld;		/* Flag indicating we are downloading thru usart */
char tchar;


usopen(io)
register struct iob *io;
{
	us_eof = 0;
	checksum = 0;
	us_first = -1;
}


usclose(io)
register struct iob *io;
{
	unsigned ch;
	unsigned val;

	if ((checksum == 0) && (us_first == -1))
		return;

	dnld++;
	for(;;) {
		if (us_eof)
			break;
		ch = getuschar();
		if (ch == -',')
			break;
		checksum += ch;
	}

	ch = getuschar();
	val = (ch<<8) + getuschar();

	while(_ttread(&tchar,1) != '!');

	dnld = 0;

	if (val != checksum) {
		printf("Checksum was %x, expected %x\n",checksum,val);
		exit(32);
	}
}


usstrategy(io,func)
register struct iob *io;
{
	char *buf;
	unsigned ch;
	int i;

	buf = io->i_ma;
	if (func == READ) {
		if (us_eof)
			return(0);
		dnld++;
		if (us_first) {
			while (_ttread(&tchar,1) != '?');
			if (_ttread(&tchar,1) != ',') {
				dnld = 0;
				exit(30);
			}
			us_first = 0;
		}
		for (i=0; i<io->i_cc; i++) {
			*buf++ = ch = getuschar();
			if (ch == -',') {
				us_eof++;
				dnld = 0;
				return(i);
			}
			checksum += ch;
		}
		dnld = 0;
		return(i);
	}
	else {
		return(write(1,io->i_ma,io->i_cc));
	}
}


getuschar() {
	unsigned chhi;
	unsigned chlo;

	chhi = _ttread(&tchar,1);
	if (chhi == '!')
		exit(31);
	if (chhi == ',')
		return(-chhi);
	chlo = _ttread(&tchar,1);
	if (chlo == '!')
		exit(31);
	if (chlo == ',')
		return(-chlo);
	return((usconv(chhi)<<4) + usconv(chlo));
}


usconv(ch)
unsigned ch;
{
	switch (ch) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		ch -= '0';
		break;
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		ch = ch - 'a' + 10;
		break;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		ch = ch - 'A' + 10;
		break;
	}
	return(ch);
}
