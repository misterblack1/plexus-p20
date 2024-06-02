static char	sccsid[] = "@(#)getargv.c	4.4";

# include <stdio.h>
#ifndef STOCKIII
#include "saio.h"
#endif

# define SCAN 0
# define FILL 1

char Ybuf[256];		/* renamed and made global cause statics are put in
			   data rather than bss and can thus not be re-init */
getargv (cmd, argvp, ff)
char *cmd;
char *(*argvp[]); {
	register unsigned count = 1;
	register char *quote, *ptr2;
	extern char *malloc ();
	extern char *strncpy ();
	extern char *strchr ();
	int pass = SCAN;
	int shift = 0;
	char *ptr1;

#ifndef STOCKIII
	char c;
	char nomoresetups;

	nomoresetups = 0;

	strncpy(myname,cmd,sizeof(myname)-1);
reprompt:
#endif
	fputs ("$$ ", stdout);
	fputs (cmd, stdout);
	putc (' ', stdout);

	if (ff)
		free ((char *) *argvp);

	ptr1 = ptr2 = &strchr (strncpy (Ybuf, cmd, sizeof (Ybuf)), '\0')[1];

	for (;;) {
		if (pass == SCAN && (ptr2-Ybuf) < sizeof (Ybuf) - 1) {
#ifndef STOCKIII
			/* a '!' introduces 'setup' commands */ 
			if(((c=getc(stdin)) == '!') && !nomoresetups) {
				dosetups(cmd);
				goto reprompt;
			}
			else {
				/* '!' no longer introduce setups */
				nomoresetups = 1;	
				ungetc(c,stdin);
			}
#endif
			fgets (ptr2, sizeof (Ybuf) - (ptr2-Ybuf) - 1, stdin);
			Ybuf[sizeof (Ybuf) - 1] = '\n';
			*strchr (ptr2, '\n') = '\0';
		}
		while ((ptr2-Ybuf) < sizeof (Ybuf) - 1) {
			while (*ptr2) {
				if (*ptr2 != ' ' && *ptr2 != '\t')
					break;
				++ptr2;
			}
			if (*ptr2 == '\0')
				break;
			if (pass == FILL)
				(*argvp)[count] = ptr2;
			++count;
			while (*ptr2) {
				if (*ptr2 == ' ' || *ptr2 == '\t')
					break;
				if (*ptr2 == '"' || *ptr2 == '\'') {
					quote = strchr (&ptr2[1], *ptr2);
					if (pass == FILL) {
						strncpy (ptr2, &ptr2[1], quote - ptr2 - 1);
						shift += 2;
					}
					ptr2 = quote;
				}
				else
					ptr2[-shift] = *ptr2;
				++ptr2;
			}
			if (pass == FILL) {
				ptr2[-shift] = '\0';
				shift = 0;
				++ptr2;
			}
		}
		if (pass == FILL)
			return (count);
		*argvp = (char **) malloc (++count * sizeof (char *));
		(*argvp)[0] = Ybuf;
		ptr2 = ptr1;
		count = 1;
		pass = FILL;
	}
}
#ifndef STOCKIII
dosetups(cmd)
char *cmd;
{
	extern *strtok();
	char setupbuf[120];
	register *scmd, *tk1, *tk2;
	int mt;

	while(gets(setupbuf) != NULL) {
		setupbuf[sizeof(setupbuf)-1]='\0';
		scmd=strtok(setupbuf,"	 ");	/* setup command name */
			/* mount setup command */
		if(!strcmp(scmd,"mount") || !strcmp(scmd,"/etc/mount")) {
			tk1=strtok(0," 	");	/* device name, eg. /dev/dk1 */
			tk2=strtok(0," 	");	/* mount directory name */
		
			if (mount (tk1, tk2) < 0)
				perror (tk1);
		}
		else if(!strcmp(scmd,"skip")) {
			tk1=strtok(0," 	");	/* name of tape file */
			tk2=strtok(0," 	");	/* no. tapes files to advance */
			if((mt=open(tk1,0)) < 0)
				perror(tk1);
			else if(srcheof(mt,atoi(tk2)) == -1) {
				printf("%s: cannot advance tape\n",tk1);
				exit(1);
			}
			else
				close(mt);
		}
				
		else if (scmd == NULL)
			/* do nothing */ ;
		else {
			printf("don't know how to do %s\n",scmd);
			printf("<ctl-D> puts you back to the %s command\n",
				cmd);
		}
		printf("! ");
	}
	printf("\n");
	return;
}
#endif
