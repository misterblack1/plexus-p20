/*
 *	acctwtmp [name [line]] >>/usr/adm/wtmp
 *	writes utmp.h record (with current time) to end of std. output
 *	name	usually name of system, defaults to null if omitted
 *	line	devname of tty, defaults to show reboot
 *
 *	acctwtmp `uname` >>/usr/adm/wtmp as part of startup
 *	acctwtmp pm >>/usr/adm/wtmp	(taken down for pm, for example)
 */
#include <stdio.h>

#ifdef V6
#include "utmp6.h"
#define BOOTSHUT	'x'
struct	utmp6	wb;
main(argc, argv)
char **argv;
{
	if (argc > 1)
		strncpy(wb.ut_name6, argv[1], sizeof(wb.ut_name6));
	if (argc > 2)
		wb.ut_tty = *argv[2];
	else
		wb.ut_tty = BOOTSHUT;
	time(&wb.ut_time6);
	fseek(stdout, 0L, 2);
	fwrite(&wb, sizeof(wb), 1, stdout);
}
#else
#include <utmp.h>
#define BOOTSHUT '~'
struct	utmp	wb;

main(argc, argv)
char **argv;
{
	if (argc > 1)
		strncpy(wb.ut_name, argv[1], sizeof(wb.ut_name));
	if (argc > 2)
		strncpy(wb.ut_line, argv[2], sizeof(wb.ut_line));
	else
		wb.ut_line[0] = BOOTSHUT;
	time(&wb.ut_time);
	fseek(stdout, 0L, 2);
	fwrite(&wb, sizeof(wb), 1, stdout);
}
#endif
