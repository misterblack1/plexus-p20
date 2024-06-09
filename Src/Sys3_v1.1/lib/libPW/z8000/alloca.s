#define ASLANG
#include <syscall.h>
#include <sys/param.h>
#include <sys/plexus.h>
//	wdptr = alloca(nbytes);
// like alloc, but automatic
// automatic free upon return from calling function
// allocates nbytes+2 (or nbytes+3 if nbtyes is odd) on stack.
// returns pointer to new top of stack - 2, thus keeping the
// "unused" word on top of stack that is in truth used by compiler. JSE

.globl _alloca
.globl __endstk
_alloca:
pop	r1, *r15		//r1 <= return point
ld	r7, *r15		//r7 <= nbytes
inc	r7, $3			//leave xtra unused word on top of stack and
res	r7, $0			//round up
sub	r15, r7
cp	r15, __endstk
jr	uge, okay		//if not into safety zone then goto okay
ld	r0, r15
sub	r0, $SAFETYZONELEN	//else grow stack and set _endstk
and	r0, $B_PAGENUM		//r0 is now beginning of a logical page
add	r0, $SAFETYZONELEN
ld	__endstk, r0
sc	$UGROW			//grow stack
jr	nc, okay		//if stack could be expanded, goto okay
sc	$GETPID			//get pid of this process
sc	$KILL			//send this process a segment violation trap
.word	SIGSEGV

okay:
ld	r7, r15
inc	r7, $4			//r7 will point to one word below top of stack
				//when parent pops the nbyte argument
jp	*r1
.data
.ascii "@(#)alloca	3.1\0"
