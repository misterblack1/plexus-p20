#include "sys/param.h"
#include "sys/tty.h"
extern char proto0inst[];
extern char proto1inst[];
extern char proto2inst[];
extern char proto3inst[];
extern int proto0stksize;
extern int proto1stksize;
extern int proto2stksize;
extern int proto3stksize;
extern char proto0data[];
extern char proto1data[];
extern char proto2data[];
extern char proto3data[];
#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/*                                                                  */
/*  function: interp                                                */
/*                                                                  */
/*  purpose: This function is invoked from the main "idle" loop.Its */
/*           purpose is to give control to the correct protocol     */
/*           script for its device.  "interp" interprets the        */
/*           protocol script and causes the script's execution.     */
/*                                                                  */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  interp();                                       */
/*                                                                  */
/*                  There are no inputs or outputs from this call.  */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

int   devicearray[5] = {
		       4,       /* device 0 uses protocol # */
		       4,       /* device 1 uses protocol # */
		       4,       /* device 2 uses protocol # */
		       4,       /* device 3 uses protocol # */
		       4,       /* device 4 uses protocol # */
};

extern  struct tty ttytab[];
extern  int thisicp;            /* our icp # (0 - 3) */

int vpmflag;
int numtimes;

gointerp()
{
       wakeup((caddr_t) &vpmflag);
}

interp()
{
	   register struct tty *tp;
	   register unit, i;
           int s;

		  /*  Loop thru all of the units (0-3) and execute the   */
		  /*  protocol specified in the devicearray table for    */
		  /*  that unit.                                         */


	    vpmflag = 1;
/*
		for(unit =0; unit < 5; unit++) {
		  tp =  &ttytab[unit];
		  tp->stacktop = 0;
		  if (devicearray[unit] == 0) tp->stacktop = proto0stksize;
		  if (devicearray[unit] == 1) tp->stacktop = proto1stksize;
		  if (devicearray[unit] == 2) tp->stacktop = proto2stksize;
		  if (devicearray[unit] == 3) tp->stacktop = proto3stksize;
		  tp->stacknext = tp->stacktop;
		  tp->pc = 0;
	        }
*/
            while(1) {

                  s = spl6();
	          timeout(gointerp, 0, vpmflag, 2000);
		  sleep((caddr_t) &vpmflag, PUSER);
                  splx( s );
/*
		  printf("interp:\n");
*/
		  unit = 0;
	          for (i=0; i<5;i++,unit++) {

#ifdef COUNT
                       profile [200]++;
#endif

			numtimes = 0;

                       tp = &ttytab[unit];

		       tp->t_dev=(devicearray[unit]<<4) | (thisicp<<6) | unit;
		       execproto(unit);
		   }
            }
}
execproto(unit)
          register unit;
{
	  register struct tty *tp;

          tp = &ttytab[unit];

          switch(devicearray[unit]) {

             case 0:

#ifdef COUNT
                    profile [201]++;
#endif
/*
		    printf("interp: p0stksiz= %x\n", proto0stksize);
*/
		    if (proto0stksize == 0) 
			  break;

		    fetch(tp, proto0inst, proto0data, proto0stksize);
                    break;

             case 1:

#ifdef COUNT
                    profile [202]++;
#endif
/*
		    printf("interp: p1stksiz= %x\n", proto1stksize);
*/
		    if (proto1stksize == 0) 
			  break;

		    fetch(tp, proto1inst, proto1data, proto1stksize);
                    break;

             case 2:

#ifdef COUNT
                    profile [203]++;
#endif
		    if (proto2stksize == 0) 
			  break;
 	     case 3:

#ifdef COUNT
                    profile [204]++;
#endif
		    if (proto3stksize == 0) 
			  break;
/*
		    fetch(tp, proto3inst, proto3data, proto3stksize);
                    break;
*/
 	     case 4:

#ifdef COUNT
                    profile [205]++;
#endif
                    break;
          }
}
