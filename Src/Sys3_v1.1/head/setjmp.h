#ifndef _JBLEN
#if vax
#define _JBLEN 10
#endif
#if pdp11
#define _JBLEN 3
#endif
#endif
typedef int jmp_buf[_JBLEN];
