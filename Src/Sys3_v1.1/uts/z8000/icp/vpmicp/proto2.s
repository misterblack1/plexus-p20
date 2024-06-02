.globl  _proto2inst
.globl  _proto2data
.globl  _proto2stksize

_proto2inst:
#include "proto2code.s"
#include "icp/opdef.h"
_proto2data: . = . + SSTACK
_proto2stksize:
.word SSTACK
