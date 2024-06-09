#include "/usr/include/sys/rm.h"
#include <stdio.h>

char *tapefname = "/dev/nrmt0";

struct {
	int c;
	char *cname;
} ctab[] = {
C_OLREW, "olrew",
C_NOP, "nop",
C_RFOREIGN, "rforeign",
C_STATUS, "status",
C_REW, "rew",
C_UNLOAD, "unload",
C_WEOF, "weof",
C_SRCHEOF, "srcheof",
C_SPACE, "space",
C_ERASE, "erase",
C_ERASEALL, "eraseall",
C_SPACEEOF, "spaceeof",
C_SRCHMEOF, "srchmeof",
0, "",
};

struct rmcmd_struct rmcmd;

main(argc, argv)
int argc;
char **argv;
{
	int fn;
	int cmd;
	int status;
	int cnt;
	int i;

	argc--;
	argv++;
	rmcmd.rm_status = -1;
	rmcmd.rm_cnt = 0;
	cnt = 0;
	cmd = C_STATUS;

	if ((fn = open(tapefname, 0)) < 0) {
		goto out;
	}
	if (argc) {
		for (i = 0; ctab[i].c > 0; i++) {
			if(0 == strcmp(ctab[i].cname, *argv)) {
				break;
			}
		}
		argc--;
		argv++;
		if (ctab[i].c == 0) {
			goto out;
		} else {
			cmd = ctab[i].c;
			if (argc) {
				if (0 == strcmp("rev", *argv)) {
					cmd |= C_REV;
					argc--;
					argv++;
				}
				if (argc) {
					cnt = atoi(*argv);
				}
			}
		}
	}

	rmcmd.rm_cmd = cmd;
	rmcmd.rm_cnt = cnt;
	if (ioctl(fn, RMPOSN, &rmcmd) < 0) {
		goto out;
	}

	out:
	close(fn);
	fprintf(stdout, "%04x", rmcmd.rm_status, rmcmd.rm_cnt);
	if (rmcmd.rm_cmd == C_RFOREIGN) {
		fprintf(stdout, " %d\n", rmcmd.rm_cnt);
	} else {
		fprintf(stdout, "\n");
	}
}
