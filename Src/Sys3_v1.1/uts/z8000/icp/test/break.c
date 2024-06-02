#include <termio.h>
#include <sys/ioctl.h>

main()
{
    int c;

    /* set process group */
    setpgrp();

    c = open("/dev/tty0",2);
    while(1) {

	    printf("wait for output, then break\n");

		/* get break request parameter */

	    ioctl(c,TCSBRK,0);

    }
}
