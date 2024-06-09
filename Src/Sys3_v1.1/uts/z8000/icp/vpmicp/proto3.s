.globl  _proto3inst
.globl  _proto3data
.globl  _proto3stksize

_proto3inst:
#include "proto3code.s"
#include "icp/opdef.h"
_proto3data: . = . + SSTACK
_proto3stksize:
.word SSTACK
