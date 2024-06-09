#include <sys/plexus.h>
#include <syscall.h>

/*
 * This is the first code run by a process. It sets up the parameters
 * for the main procedure, calls it, and cleans up after it exits.
 * The kernel sets up the stack prior to running this code as follows:
 *
 * 	1)	*r15 contains the parameter count (argc)
 *
 *	2)	2(r15) and above contains a list of pointers to the
 *		arguments. This list is (argc + 1) * 2 bytes
 *		long including the null pointer at the end of the list.
 *
 *	3)	The next higher byte after the argv list is the start of
 *		the environment string. This string is null byte terminated.
 *
 *
 * This version of the start off code defines a global which is used for
 * stack expansion since the hardware can't handle it automatically.
 * The global, __endstk, contains a data address. If the stack is
 * going to grow below this address the next lower page of space is
 * allocated by the ugrow system call.
 *
 * This particular version of the startoff code includes automatic set up
 * calls which cause the process to be monitored. This source also includes
 * an `_exit' procedure which terminates monitoring in a clean way.
 */

.globl	start			/* entrance to start off code */
.globl	_main			/* main procedre of user process */
.globl	_exit			/* procedure called after main returns */
.globl	_environ		/* global to save pointer to environ string */
.globl	__endstk		/* address of top of safety zone */
.globl	_monitor		/* function which starts up monitoring */
.globl	_sbrk			/* function which gets a block of memory */
.globl	_etext			/* address of the end of the text area */
.globl	__cleanup		/* users routine called by `_exit' */
.globl	countbase		/* buffer address of monitor buffer */

#define	MONNFUNC	300	/* number of procedures monitored */

start:
ld	r1, *r15		/* r1 = argc */
lda	r2, 2(r15)		/* r2 = argv */
ld	r3, r1
inc	r3
add	r3, r3			/* r3 = length of argv list */
add	r3, r2			/* r3 = envp */
ld	_environ, r3		/* save for use by process */
ld	r0, r15			/* r0 = stk ptr */
sub	r0, $SAFETYZONELEN	/* r0 = ptr to bottom of safety zone */
and	r0, $B_PAGENUM		/* r0 = ptr to bottom of safety zone page */
add	r0, $SAFETYZONELEN	/* r0 = last address before new safety zone */
ld	__endstk, r0		/* save for use by csav */
sc	$UGROW			/* extend stack so safety zone is covered */
push	*r15, r3		/* push envp arg for main */
push	*r15, r2		/* push argv arg for main */
push	*r15, r1		/* push argc arg for main */
clr	r14			/* null previous stack frame ptr */

ld	r1, $_etext		/* r1 = temp to calc size of profile buf */
sub	r1, $eprol
add	r1, $7
srl	r1, $3
and	r1, $0x1fff		/* r1 = size of text area / 8 */
push	*r15, $MONNFUNC		/* monitor parm - number of functions */
add	r1, $(3 * (MONNFUNC + 1))
push	*r15, r1		/* monitor parm - words in mon buffer */
sll	r1
push	*r15, r1		/* sbrk parm - number of bytes wanted */
call	_sbrk			/* get a block of memory for monitor */
inc	r15, $2			/* delete sbrk parameter from stack */
cp	r7, $-1
jr	eq, nomem		/* if (not enough memory) goto nomem */
push	*r15, r7		/* monitor parm - buffer address */
add	r7, $6
ld	countbase, r7		/* save buffer address for later use */
push	*r15, $_etext		/* monitor parm - last profile address */
push	*r15, $eprol		/* monitor parm - first profile address */
call	_monitor		/* start monitoring */
inc	r15, $10		/* delete monitor parameters from stack */
call	_main			/* start user process */
inc	r15, $6			/* delete parameters to main */
ld	r0, r7			/* r0 = return from main */
push	*r15, r0		/* push arg for exit */ 
call	_exit			/* go clean up */

nomem:
push	*r15, $(errmsgend - errmsg)	/* length of string */
push	*r15, $errmsg		/* address of buffer */
push	*r15, $2		/* stderr file number */
call	_write			/* write out error message */
inc	r15, $6			/* delete write call parms */

_exit:
ldk	r0, $LENSAVEREG
call	csav
call	__cleanup
push	*r15, $0
call	_monitor
inc	r15, $2
ld	r0, 4(r14)
sc	$EXIT

eprol:

.data
. = . + 2			/* referent of null ptr */

errmsg:
.ascii	"No space for monitor buffer\n"
.even

errmsgend:

.bss
_environ:
. = . + 2			/* global for environment ptr */

countbase:
. = . + 2			/* global address of monitor buffer */

__endstk:
. = . + 2			/* global for safety zone ptr */
