/*
 * This program dials a vadic 3451 modem.
 * The first parameter is the name of a tty port attached to the modem
 * the second parameter is the speed to use, 300 or 1200,
 * and the third parameter is the phone number that is fed to the vadic.
 * The program will provide various exit codes depending on what happened:
 *
 *      exit(0) - Connection was made.
 *      exit(1) - Parameter error.
 *      exit(2) - Can't use dialer tty in some way.
 *      exit(3) - Dialer indicated no answer.
 *      exit(4) - Dialer not responding.
 */

#include <termio.h>
#include <stdio.h>
#include <signal.h>

FILE *in;
FILE *out;
int onalarm();
char prompt[80];
struct termio x;

main(argc, argv)
int argc;
char **argv;
{
        struct termio s;
        int num;
        int l;
	int cnt;

        assert(1, argc == 4);
        signal(SIGALRM, onalarm);
        alarm(120);  /* time out if either open hangs */
        assert(2, (in = fopen(argv[1], "r")) != NULL);
        assert(2, (out = fopen(argv[1], "w")) != NULL);
        alarm(0);
        ioctl(fileno(out), TCGETA, &x);
        assert(2, 0 == ioctl(fileno(out), TCGETA, &s));
        s.c_iflag = IGNBRK | IGNPAR | ISTRIP | IXON | IXOFF;
	s.c_oflag = ONLCR;
	s.c_cflag = CS8 | HUPCL | CREAD | CLOCAL;
	s.c_lflag = ECHOK;
        assert(1, (strcmp(argv[2], "300") == 0) | (strcmp(argv[2], "1200") == 0));
        if (strcmp(argv[2], "300") == 0) {
                s.c_cflag |= B300 ;
        } else {
                s.c_cflag |= B1200;
        }
        assert(2, 0 == ioctl(fileno(out), TCSETA, &s));
	cnt = 0;
	fprintf(out, "I\r");
	fflush(out);
	sleep(2);
	for (;;) {
        	fprintf(out, "\005");
        	fflush(out);
		sleep(2);
        	fprintf(out, "\r");
        	fflush(out);
        	waitforprompt();
		if (strcmp(prompt, "HELLO: I'M READY\r\n") == 0) {
			break;
		} else {
			if (strcmp(prompt, "INVALID\r\n") == 0) {
				break;
			}
		}
		if (cnt++ == 10)
			exit(4);
	}
        fprintf(out, "D\r");
        fflush(out);
        waitforprompt();
	while (strcmp(prompt, "*NUMBER?\r\n") != 0)
		waitforprompt();
        fprintf(out, "%s\r", argv[3]);
        fflush(out);
        waitforprompt();
        fprintf(out, "\r");
        fflush(out);
        waitforprompt();
	ioctl(fileno(out), TCSETA, &x);
        if (strcmp(prompt, "DIALING:  ON LINE\r\n") == 0) {
                exit(0);
        } else {
                exit(3);
        }
}

waitforprompt()
{

        alarm(120);
        assert(2, NULL != fgets(prompt, 80, in));
        alarm(0);
        sleep(2);  /* god knows why this is necessary but it is at 300 baud */
}

onalarm()
{
	ioctl(fileno(out), TCSETA, &x);
        exit(4);  /* dialer dead */
}

assert(ecode, cond)
int ecode;
int cond;
{
        if (!cond) {
		ioctl(fileno(out), TCSETA, &x);
                exit(ecode);
        }
}
