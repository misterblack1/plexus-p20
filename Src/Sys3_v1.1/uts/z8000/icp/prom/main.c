/*
 * main program for prom functions
 */

#include "/p3/usr/include/icp/sioc.h"	/* icp specific */
#include "/p3/usr/include/icp/siocprom.h"	/* icp specific */
#include "/p3/usr/include/icp/ctc.h"	/* icp specific */
#include "/p3/usr/include/icp/sio.h"	/* icp specific */

#define	LONGTIME	5000		/* slow polling time ~100 ms */
#define	SHORTTIME	500		/* fast polling time ~10 ms */
#define	WAITTIME	50		/* hysterisis for fast to slow poll */


main()
{
	struct cmd cmd;
	long cmdadx;
	int	speed;
	long time;
	int delta, tchar;
	long i;


/*	Program CTC timer for multibus grant.  */

	out_local( P796TIMER, CTIME | CRESET | CCTRL );
	out_local( P796TIMER, 250 );

/* 	Program CTC for channel attention.  */

	out_local( P796CATTN, CCTR | CUPCLK | CTIME | CRESET | CCTRL );
	out_local( P796CATTN, 2 );

/* 	Now wait for a channel attention before doing anything. */

	while ( (in_local( P796CATTN) & 0xff) == 2 ) { }


	cmdadx = (((long) in_local(PWUA)) & 0xffff) << 4;


	while ( 1 ) {
		for ( i = 1000; i > 0; i-- ) {
		}
		mvfrommb( &cmd, cmdadx, sizeof(cmd) );
		if ( cmd.c_gate != 0 ) {
			speed = cmd.c_flags & 07;
			cmd.c_gate = 0;
			cmd.c_flags = 0;
			tchar = cmd.c_char;
			cmd.c_char = 0;
			mvtomb( (cmdadx + 2), &cmd.c_command, (sizeof(cmd) - 2) );
			mvtomb( cmdadx, &cmd, 2 );
			if ( tchar != 0xff )
				out_local( PCMD, INT796 );
			break;
		}
	}
	initsio( speed );
	delta = 0;
	time = LONGTIME;

	while (1) {
		for (i = time; i > 0; i--) {
		}
		mvfrommb(&cmd, cmdadx, sizeof(cmd));
		if (cmd.c_gate == 0) {
			delta++;
			if (delta > WAITTIME) {
				time = LONGTIME;
			}
		} else {
			delta = 0;
			time = SHORTTIME;
			switch(cmd.c_command) {

			case WBLK:
				mvfrommb(cmd.c_lcladx, cmd.c_mbadx, cmd.c_len);
				break;

			case RBLK:
				mvtomb(cmd.c_mbadx, cmd.c_lcladx, cmd.c_len);
				break;

			case JMP:
				break;

			case WCHAR:
				putchar(cmd.c_char);
				break;

			case RCHAR:
				cmd.c_char = getchar();
				break;
			case CRDY:
				cmd.c_char = ( in_local( PSIO0CMD ) & RXRDY );
				break;
			}

			cmd.c_gate = 0;
			mvtomb( (cmdadx + 2), &cmd.c_command, (sizeof(cmd) - 2) );
			mvtomb(cmdadx, &cmd, 2 );
			if (cmd.c_flags & CLRINT) {
				out_local(PCMD, INT796);
			}
			if (cmd.c_command == JMP) {
				jmp(cmd.c_lcladx);
			}
		}
	}
}
