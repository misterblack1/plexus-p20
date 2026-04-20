/* SID @(#)opt.h	5.1 */
/* @(#)opt.h	6.1 */
#if MESG==0
msgsys(){nosys();}
msginit(){return(0);}
#endif

#if SEMA==0
semsys(){nosys();}
seminit(){}
semexit(){}
#endif

#if SHMEM==0
shmsys(){nosys();}
shmexec(){}
shmexit(){}
shmfork(){}
shmreset(){}
#endif
