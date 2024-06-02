#include	"crash.h"
#include	"cmd.h"
#include	"setjmp.h"

int	kmem;
long	ka6 = -1;
int	r5;
long	delta = 0x100000;	/* physical memory starts here for /dev/mem */
long	bufbase = 0;		/* base of buffers if specified */
char	*namelist = "/sys3";
char	*dumpfile = "/dev/mem";
char	*mapfile = "/dev/liomem";
char	line[256], *p = line;
unsigned maps[0x800];		/* memory maps */
struct	var	v;
int	tok;
jmp_buf	jmp;
struct	nlist	*File, *Inode, *Mount, *Swap, *Core, *Proc,
		*Sys, *Time, *Panic, *Etext, *Text, *V,
		*Buf, *End, *Callout, *Lbolt;

main(argc, argv)
	int	argc;
	char	**argv;
{
	char	*token();
	long	atol();
	int	sigint();
	register  struct  tsw	*tp;
	register  char  *c, *arg;
	int	cnt, units, r, prdef = OCTAL;
	long	addr;
	struct	nlist	*sp;
	struct	prmode	*prptr;
	extern	unsigned  ttycnt;

	switch(argc) {
	default:
		fatal("usage: crash  [ dump ]  [ namelist ]  [ ka6 ]");
	case 5:
		if(*argv[4] != '-')
			bufbase = atol(argv[4]);
	case 4:
		if(*argv[3] != '-')
			ka6 = atol(argv[3]) << 6;
	case 3:
		if(*argv[2] != '-')
			namelist = argv[2];
	case 2:
		if(*argv[1] != '-') {
			dumpfile = argv[1];
			delta = 0;
		}
	case 1:

		;
	}

	init();
	setjmp(jmp);

	for(;;) {
		c = token();
		for(tp = t; tp->t_sw != 0; tp++)
			if(strcmp(tp->t_nm, c) == 0)
				break;
		switch(tp->t_sw) {

		default:
		case NULL:
			printf("eh?\n");
			while(token() != NULL);
			continue;

		case UAREA:
			pruarea();
			while(token() != NULL);
			continue;

		case STACK:
			prstack();
			while(token() != NULL);
			continue;

		case TRACE:
			if(token() != NULL) {
				prtrace(1);
				while(token() != NULL);
				continue;
			}
			prtrace(0);
			continue;

		case FILES:
			printf("SLOT  REF  INODE  FLAGS\n");
			if((arg = token()) == NULL)
				for(cnt = 0; cnt < v.v_file; cnt++)
					prfile(cnt, 0);
			else do {
				prfile(atoi(arg), 1);
			} while((arg = token()) != NULL);
			continue;

		case KA6:
			if((arg = token()) == NULL)
				printf("ka6: %lo\n", ka6);
			else {
				ka6 = atol(arg) << 6;
				while(token() != NULL);
			}
			continue;

		case R5:
			if((arg = token()) == NULL)
				printf("r5: %6.6o\n", r5);
			else {
				r5 = atoi(arg);
				while(token() != NULL);
			}
			continue;

		case INODE:
			printf("SLOT MAJ  MIN INUMB REF LINK  UID  ");
			printf("GID   SIZE   MODE  SMAJ SMIN FLAGS\n");
			if((arg = token()) == NULL)
				for(cnt = 0; cnt < v.v_inode; cnt++)
					prinode(cnt, 0, 0);
			else if(*arg == '-')
				if((arg = token()) == NULL)
					for(cnt = 0; cnt < v.v_inode; cnt++)
						prinode(cnt, 1, 0);
				else
					do prinode(atoi(arg), 1, 1);
					while((arg = token()) != NULL);
			else
				do prinode(atoi(arg), 0, 1);
				while((arg = token()) != NULL);
			continue;

		case PROC:
			printf("SLT ST PID   PPID  PGRP  UID PRI CPU");
			printf("  ADDR  EVENT NAME     FLAGS\n");
			if((arg = token()) == NULL)
				for(cnt = 0; cnt < v.v_proc; cnt++)
					prproc(cnt, 0, 0, 0);
			else if(*arg == '-') {
				r = arg[1] == 'r' ? 0 : 1;
				if((arg = token()) == NULL)
					for(cnt = 0; cnt < v.v_proc; cnt++)
					prproc(cnt, r, !r, 0);
				else
					do prproc(atoi(arg), r, !r, 1);
					while((arg = token()) != NULL);
			} else
				do prproc(atoi(arg), 0, 0, 1);
				while((arg = token()) != NULL);
			continue;

		case TTY:
			printf("SLOT RAW DEL CAN OUT  PGRP COL STATE\n");
			l1: if((arg = token()) == NULL)
				for(cnt = 0; cnt < ttycnt; cnt++)
					prtty(cnt, 0, 0);
			else if(*arg == '-')
				if((arg = token()) == NULL)
					for(cnt = 0; cnt < ttycnt; cnt++)
						prtty(cnt, 1, 0);
				else do prtty(atoi(arg), 1, 1);
					while((arg = token()) != NULL);
			else if(*arg >= '0' && *arg <= '9')
				do prtty(atoi(arg), 0, 1);
				while((arg = token()) != NULL);
			else if(!settty(arg)) {
				while(token() != NULL);
				continue;
			} else goto l1;
			continue;

		case TEXT:
			printf("SLOT  INODE  REF  CREF  CORE  ");
			printf("DISK  SIZE  FLAGS\n");
			if((arg = token()) == NULL)
				for(cnt = 0; cnt < v.v_text; cnt++)
					prtext(cnt, 0);
			else do {
				prtext(atoi(arg), 1);
			} while((arg = token()) != NULL);
			continue;

		case MOUNT:
			printf("SLOT  MAJ  MIN  INODE  ");
			printf("BUF  VOLUME  PACK   BLOCKS INODES");
			printf("  BFREE  IFREE\n");
			if((arg = token()) == NULL)
				for(cnt = 0; cnt < v.v_mount; cnt++)
					prmount(cnt, 0);
			else do {
				prmount(atoi(arg), 1);
			} while((arg = token()) != NULL);
			continue;

		case TNS:
			while((arg = token()) != NULL) {
				if(atoi(arg) == -1)
					continue;
				sp = search(atoi(arg), 042, 042);
				if(sp == 0)
					printf("\tno match\n");
				else if(sp->n_name[0] == '_')
					printf("\t%.7s\n",&sp->n_name[1]);
				else
					printf("\t%.8s\n",sp->n_name);
			}
			continue;

		case DS:
			while((arg = token()) != NULL) {
				if(atoi(arg) == -1)
					continue;
				sp = search(atoi(arg), 043, 044);
				if(sp == 0)
					printf("\tno match\n");
				else if(sp->n_name[0] == '_')
					printf("\t%.7s", &sp->n_name[1]);
				else
					printf("\t%.8s", sp->n_name);
				if(sp)
					printf(" + %u.\n", atoi(arg) -
						sp->n_value);
			}
			continue;

		case Q:
			printf("PWB Dump Analyzer\n");
			printf("usage: crash [dump] [namelist] [ka6]\n");
			printf("available commands:\n\n");
			for(tp = t; tp->t_sw != 0; tp++)
				if(tp->t_dsc != 0)
					printf("%s\t%s\n",tp->t_nm,tp->t_dsc);
			while(token() != NULL);
			continue;

		case STAT:
			prstat();
			while(token() != NULL);
			continue;

		case BUFHDR:
			printf("BUF MAJ  MIN   BLOCK FLAGS\n");
			if((arg = token()) == NULL)
				for(cnt = 0; cnt < v.v_buf + v.v_sabuf; cnt++)
					prbufhdr(cnt);
			else do prbufhdr(atoi(arg));
				while((arg = token()) != NULL);
			continue;

		case BUFFER:
			if((arg = token()) == NULL)
				for(cnt = 0; cnt < v.v_buf; cnt++)
					prbuffer(cnt, prdef);
			else if(*arg >= '0' && *arg <= '9')
				do prbuffer(atoi(arg), prdef);
				while((arg = token()) != NULL);
			else {
				for(prptr = prm; prptr->pr_sw != 0; prptr++)
					if(!strcmp(arg, prptr->pr_name))
						break;
				if(prptr->pr_sw == 0) {
					error("invalid mode");
					while(token() != NULL);
					continue;
				}
				prdef = prptr->pr_sw;
				if((arg = token()) == NULL)
					for(cnt = 0; cnt < v.v_buf; cnt++)
						prbuffer(cnt, prptr->pr_sw);
				else do prbuffer(atoi(arg), prptr->pr_sw);
					while((arg = token()) != NULL);
			}
			continue;

		case TOUT:
			printf("FUNCTION ARGUMENT TIME\n");
			prcallout();
			while(token() != NULL);
			continue;

		case NM:
			while((arg = token()) != NULL)
				prnm(arg);
			continue;

		case OD:
			if((arg = token()) == NULL) {
				error("symbol expected");
				continue;
			}
			if(arg[0] >= '0' && arg[0] <= '9')
				addr = atol(arg);
			else if((sp = nmsrch(arg)) == NULL) {
				printf("symbol not found\n");
				while(token() != NULL);
				continue;
			} else
				addr = (long)sp->n_value;
			if((arg = token()) == NULL) {
				units = 1;
				arg = "octal";
			} else {
				units = atoi(arg);
				if(units == -1) {
					while(token() != NULL);
					continue;
				}
				if((arg = token()) == NULL)
					arg = "octal";
				else
					while(token() != NULL);
			}
			prod(addr, units, arg);
			continue;

		case MAP:
			while((arg = token()) != NULL) {
				prmap(arg);
				putc('\n', stdout);
			}
			continue;

		case VAR:
			prvar();
			while(token() != NULL);
			continue;

		case QUIT:
			exit(0);
		}
	}
}

sigint()
{
	char  *token();

	signal(SIGINT, sigint);
	p = line;
	tok = 1;
	printf("\neh?\n");
	line[0] = '\0';
	while(token() != NULL);
	longjmp(jmp, 0);
}

char	*
token()
{
	register  char  *cp;

	for(;;)
		switch(*p) {
		case '\0':
		case '\n':
			if(tok != 0) {
				tok = 0;
				return(NULL);
			}
			printf("> ");
			p = line;
			if(fgets(line, 256, stdin) == NULL)
				fatal("end of file on input");
			if(line[0] == '!') {
				system(&line[1]);
				line[0] = '\0';
			}
			continue;

		case ' ':
		case '\t':
			p++;
			continue;

		default:
			tok++;
			cp = p;
			while(*p!=' ' && *p!='\t' && *p!='\n')
				p++;
			*p++ = '\0';
			return(cp);
	}
}
