#define JOBS 0
#define SYMLINKS 0
#define DIRENT 0
#define UDIR 1
#define ATTY 0
#define SHORTNAMES 0
#define SYSV

#if SHORTNAMES
#include "shortnames.h"
#endif

#ifdef __STDC__
typedef void *pointer;
#ifndef NULL
#define NULL (void *)0
#endif
#else
#define const
#define volatile
typedef char *pointer;
#ifndef NULL
#define NULL 0
#endif
#endif

#ifdef DEBUG
#define TRACE(param)	trace param
#else
#define TRACE(param)
#endif
#define STATIC static
#define MKINIT
extern char nullstr[];
