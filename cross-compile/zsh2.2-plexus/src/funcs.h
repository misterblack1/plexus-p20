struct asgment;
struct utmp;

/*
 * Minimal function declarations for SVR2 (limited symbol table).
 * Only declare functions returning non-int types.
 */

char *mktemp();
#ifndef HAS_STDLIB
char *malloc();
char *realloc();
char *calloc();
#endif
char *ttyname();
char *getenv();

/* String/memory functions used across zsh */
char *ztrdup(), *ztrstr(), *tricat(), *dyncat();
char *spacejoin(), *join(), *colonjoin();
char *dupstring(), *strdup();
char *zgetwd(), *findpwd(), *fixpwd();
char *zgetenv(), *getsparam();
char *putprompt(), *makequote();
char *gettemp(), *namedpipe();
char *getstrvalue(), *getsparamval();
char *findcmd(), *getfullpath();
char *hstrnstr(), *makehstr();
char *getevent(), *qgetevent(), *quietgetevent();
char *gethere(), *hdynread(), *hdynread2();
char *convamps(), *fgetline(), *dynread();
char *getkeystring(), *get_comp_string();
char *getargs(), *getpermtext(), *getjobtext();
char *getoutputfile(), *getinproc(), *getoutproc();
char *replenv(), *addenv(), *mkenvstr();
char *gethome(), *dstackent(), *xsymlink();
char *hp_alloc(), *hp_realloc(), *hp_concat();
char *getparen(), *spname(), *escsubst();
char *hgets(), *hwadd();
char *getcurcmd();

/* void functions used across files */
void freecmdnam(), freecompctl(), freestr(), freeanode();
void freestruct();

/* termcap */
extern char PC, *BC, *UP;
extern short ospeed;
extern int tgetent();
extern int tgetnum();
extern int tgetflag();
extern char *tgetstr();
extern char *tgoto();
extern int tputs();

/* char ** returning functions */
char **arrdup(), **colonsplit(), **mkarray();
char **slashsplit(), **spacesplit(), **get_user_var();
char **getaparam(), **getarrvalue(), **makecline();
char **arrgetfn(), **arrvargetfn(), **nullgetfn();
char **pathagetfn(), **pathgetfn();
extern void freepm();
extern int dup2();
