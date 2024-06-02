/*
 * Line Discipline Switch table
 */
#include "sys/conf.h"

extern	nulldev();
extern	ttopen(), ttclose(), ttread(), ttwrite(), ttin(), ttout();
struct linesw linesw[] = {
/* 0 */	ttopen, ttclose, ttread, ttwrite, nulldev, ttin, ttout, nulldev,
};
int	linecnt = 1;
