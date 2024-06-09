.globl  _proto1inst
.globl  _proto1data
.globl  _proto1stksize

_proto1inst:
#include "proto1code.s"
#include "icp/opdef.h"
_proto1data: . = . + SSTACK
_proto1stksize:
.word SSTACK
