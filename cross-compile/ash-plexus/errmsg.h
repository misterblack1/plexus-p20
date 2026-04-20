#define E_OPEN 01
#define E_CREAT 02
#define E_EXEC 04

#ifdef __STDC__
char *errmsg(int, int);
#else
char *errmsg();
#endif
