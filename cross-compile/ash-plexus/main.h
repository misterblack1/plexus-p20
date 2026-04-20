extern int rootpid;		/* pid of main shell */
extern int rootshell;		/* true if we aren't a child of the main shell */

#ifdef __STDC__
void readcmdfile(char *);
void cmdloop(int);
#else
void readcmdfile();
void cmdloop();
#endif
