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
 */

.globl	start			/* entrance to start off code */
.globl	_main			/* main procedre of user process */
.globl	_exit			/* procedure called after main returns */
.globl	_environ		/* global to save pointer to environ string */
.globl	__endstk		/* address of top of safety zone */

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
call	_main			/* start user process */
inc	r15, $6			/* delete parameters to main */
ld	r0, r7			/* r0 = return from main */
push	*r15, r0		/* push arg for exit */ 
call	_exit			/* call exit cleanup - this should not return */
sc	$EXIT			/* in case exit call returns */

.data
. = . + 2			/* referent of null ptr */

.bss
_environ:
. = . + 2			/* global for environment ptr */

__endstk:
. = . + 2			/* global for safety zone ptr */
