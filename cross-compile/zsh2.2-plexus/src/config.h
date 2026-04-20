/* config.h for Plexus P/20 - AT&T UNIX System V Release 2, MC68010 */
#define PLEXUS_SVR2

/* We do NOT define SYSV because the SYSV block in zsh.h pulls in
   sys/bsdtypes.h, sys/limits.h, sys/sioctl.h, sys/dirent.h which
   don't exist on this system. We handle SVR2 specifics individually. */
/* #define SYSV */

/* Plexus has TERMIO, not TERMIOS */
#define TERMIO
/* #define TERMIOS */
/* #define TTY_NEEDS_DRAINING */
/* #define CLOBBERS_TYPEAHEAD */

/* #define HAS_STDLIB */

#define HAS_STRING

/* #define HAS_LOCALE */

/* Plexus utmp does not have ut_host */
/* #define UTMP_HOST */

/* SVR2 has wait() only, no waitpid */
/* #define WAITPID */

/* No select() on SVR2 */
/* #define HAS_SELECT */
/* #define HAS_SYS_SELECT */

/* #define NO_FIFOS */

/* No strftime on SVR2 */
/* #define HAS_STRFTIME */

/* No POSIX terminal functions on SVR2 */
/* #define HAS_TCSETPGRP */
/* #define HAS_TCCRAP */

/* SVR2 has setpgrp() with no args, not setpgid() */
/* #define HAS_SETPGID */

/* #define HAS_SIGRELSE */
/* #define HAS_RFS */

/* No getrusage/wait3 on SVR2 */
/* #define HAS_RUSAGE */

/* SVR2 signal handlers return int, not void */
/* #define SIGVOID */

/* SVR2 resets signal handlers to SIG_DFL after each delivery */
#define RESETHANDNEEDED

#ifdef SIGVOID
#define HANDTYPE void
#else
#define HANDTYPE int
#define INTHANDTYPE
#endif

#define HOSTTYPE "m68k"
#define DEFFCEDIT "vi"
#define WTMP_FILE "/etc/wtmp"
#define UTMP_FILE "/etc/utmp"
#define DEFTMPPREFIX "/tmp/zsh"
#define GLOBALZSHRC "/etc/zshrc"
#define GLOBALZLOGIN "/etc/zlogin"
#define GLOBALZPROFILE "/etc/zprofile"
#define DEFAULT_HISTSIZE 30

/* SVR2 cc may not support void* -- use char* */
typedef char *vptr;

/* No job control signals on Plexus SVR2 (no SIGTSTP/SIGSTOP/SIGCONT) */
/* #define JOB_CONTROL */

/* Stub signal numbers for code that references job control signals
   unconditionally. These map to SIGUSR1 so signal() calls won't crash,
   but the signals are never actually delivered. */
#ifndef SIGTSTP
#define SIGTSTP 99
#endif
#ifndef SIGSTOP
#define SIGSTOP 99
#endif
#ifndef SIGCONT
#define SIGCONT 99
#endif
#ifndef SIGTTIN
#define SIGTTIN 99
#endif
#ifndef SIGTTOU
#define SIGTTOU 99
#endif

/* SVR2 uses SIGCLD, not SIGCHLD */
#ifndef SIGCHLD
#define SIGCHLD SIGCLD
#endif

/* lstat doesn't exist on SVR2 (no symlinks) */
#define lstat stat

/* No EWOULDBLOCK on SVR2, use EAGAIN */
#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

/* Provide MAXPATHLEN if not defined */
#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

/* Plexus has no TIOCSPGRP/TIOCGPGRP - stub them out */
#ifndef TIOCSPGRP
#define TIOCSPGRP 0
#endif
#ifndef TIOCGPGRP
#define TIOCGPGRP 0
#endif
#ifndef BUFSIZ
#define BUFSIZ 1024
#endif
