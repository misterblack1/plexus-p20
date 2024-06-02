#include "sys/param.h"
#include "sys/tty.h"
#include "icp/sio.h"
#include "icp/icp.h"
#include "icp/opdef.h"
#include "icp/crctab.h"
#include "icp/atoetbl.h"
#include "icp/etoatbl.h"
#ifdef COUNT
extern int profile[];
#endif
#ifdef WAITING
extern int profile[];
#endif

extern int timerid;
extern int numtimes;
extern int devicearray[];
extern int onlyone;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*                                                     */
/* The following function is invoked as a result of an */
/* expired "timer" request.  This differs from the     */
/* "timertn" routine which deals with "timeout"        */
/* requests.                                           */
/*                                                     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

tim2func(tp)
register struct tty *tp;
{
     tp->v_tim2id = 0;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*                                                     */
/* The following function is invoked as a result of an */
/* expired timeout request.  This differs from the     */
/* "timer" requests.                                   */
/*                                                     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

int	savestkactv;

timertn(tp)
register struct tty *tp;
{
	savestkactv = tp->stackactive;

	tp->v_iflag |= 02;

}
/*
int pcdata;

chkit()
{
	if(pcdata < 0 || pcdata > 0xaa) {
		printf("pcdata BAD! pcdata=%x", pcdata);
		debug();
	}
}
*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/*                                                                  */
/*  function: fetch                                                 */
/*                                                                  */
/*  purpose: This function executes the protocol scripts by         */
/*           interpreting the protocols scripts.                    */
/*                                                                  */
/*  calling syntax:                                                 */
/*                  fetch(tp,protoinst,protodata,sstack)            */
/*                                                                  */
/*                                                                  */
/*                  where: tp is the pointer to the current entry   */
/*                               into "ttytab" for this current     */
/*                               device being serviced.             */
/*                                                                  */
/*                  where: protoinst is the array of protocol       */
/*                                   script instructions.           */
/*                                                                  */
/*                  where: protodata is the array of protocol       */
/*                                   script data storage.  These    */
/*                                   data are referenced by the     */
/*                                   protoinst relative to the      */
/*                                   sstack address.                */
/*                                                                  */
/*                  where: sstack is the stack length.              */
/*                                                                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

fetch(tp,protoinst,protodata,sstack)

register struct tty *tp;
char protoinst[];
char protodata[];
int sstack;
{
	int pcdata;
	int  temp, temp1;
	char array[4];
	int s;

doagain:
	if(tp->v_iflag & 02 ) {
		goto istimout;
	}
/*
     printf("fetch: inst[tp->pc]= %x", protoinst[tp->pc]);
     printf("fetch: tp->pc= %x", tp->pc);
*/
/*
	if(tp->pc > 0x3ee || tp->pc < 0) {
     		printf("fetch: tp->pc= %x", tp->pc);
		printf("tp->pc BAD!");
		debug();
	}
*/
     temp = protoinst[tp->pc]&0260;
	  
     if((temp == JMP) || (temp == JMPEQZ) || (temp == JMPNEZ) || (temp == CALL))
	  goto jmpinst;

     temp = tp->pc;
     temp1 = tp->pc;
     temp++;
/*
     printf("fetch: protoinst[temp]= %x", protoinst[temp]);
     printf("fetch: sstack =%x", sstack);
*/
     pcdata = (protoinst[temp] - sstack) & 0xff;
/*
     printf("fetch: pcdata= %x", pcdata);
*/

     tp->pc++;
     switch (protoinst[temp1]) {

       case AND:
/*
	    chkit();
*/
	    tp->ac &= protodata[pcdata];
	    tp->pc++;
	    break;

       case OR:
/*
	    chkit();
*/
	    tp->ac |= protodata[pcdata];
	    tp->pc++;
	    break;

       case MOV:
/*
	    chkit();
*/
	    tp->ac = protodata[pcdata];
/*		The following prints the crc for the rje protocol script */
/*
	    if(tp->pc == 727 || tp->pc == 735) {
		printf("MOV:crc=%x, AC=%x", protodata[pcdata], tp->ac);
	    }
*/
	    tp->pc++;
	    break;

       case XOR:
/*
	    chkit();
*/
	    tp->ac ^= protodata[pcdata];
	    tp->pc++;
	    break;

       case BIC:
/*
	    chkit();
*/
	    tp->ac &= ~protodata[pcdata];
	    tp->pc++;
	    break;

       case ADD:
/*
	    chkit();
*/
	    tp->ac += protodata[pcdata];
	    tp->pc++;
	    break;

       case SUB:
/*
	    chkit();
*/
	    tp->ac -= protodata[pcdata];
	    tp->pc++;
	    break;

       case LSH:
/*
	    chkit();
*/
	    tp->ac <<= protodata[pcdata];
	    tp->pc++;
	    break;

       case RSH:
/*
	    chkit();
*/
	    tp->ac >>= protodata[pcdata];
	    tp->pc++;
	    break;

       case TSTEQL:
/*
	  	chkit();
*/
	    if(tp->ac == protodata[pcdata]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    tp->pc++;
	    break;

       case TSTNEQ:
/*
	    chkit();
*/
	    if(tp->ac != protodata[pcdata]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    tp->pc++;
	    break;

       case TSTGTR:
/*
	    chkit();
*/
	    if(tp->ac > protodata[pcdata]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    tp->pc++;
	    break;

       case TSTLSS:
/*
	    chkit();
*/
	    if(tp->ac < protodata[pcdata]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    tp->pc++;
	    break;

       case TSTGEQ:
/*
	    chkit();
*/
	    if(tp->ac >= protodata[pcdata]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    tp->pc++;
	    break;

       case TSTLEQ:
/*
	    chkit();
*/
	    if(tp->ac <= protodata[pcdata]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    tp->pc++;
	    break;

       case MOVI:

	    tp->ac = protoinst[tp->pc++];

	    break;

       case ANDI:
/*
	    printf("ANDI:");
*/
	    tp->ac &= protoinst[tp->pc++];
	    break;

       case ORI:

	    tp->ac |= protoinst[tp->pc++];
	    break;

       case XORI:

	    tp->ac ^= protoinst[tp->pc++];
	    break;

       case BICI:

	    tp->ac &= ~protoinst[tp->pc++];
	    break;

       case ADDI:

	    tp->ac += protoinst[tp->pc++];
	    break;

       case SUBI:

	    tp->ac -= protoinst[tp->pc++];
	    break;

       case LSHI:

	    tp->ac <<= protoinst[tp->pc++];
	    break;

       case RSHI:

	    tp->ac >>= protoinst[tp->pc++];
	    break;

       case TSTEQLI:
/*
    printf("TSTEQLI:AC=%x, INST=%x", tp->ac&0xff,protoinst[tp->pc]&0xff);
*/
	    if((tp->ac & 0xff) == (protoinst[tp->pc++] & 0xff)) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    break;

       case TSTNEQI:

	    if((tp->ac & 0xff) != (protoinst[tp->pc++] & 0xff)) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    break;

       case TSTGTRI:

	    if(tp->ac > protoinst[tp->pc++]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    break;

       case TSTLSSI:

	    if(tp->ac < protoinst[tp->pc++]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    break;

       case TSTGEQI:

	    if(tp->ac >= protoinst[tp->pc++]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    break;

       case TSTLEQI:

	    if(tp->ac <= protoinst[tp->pc++]) {
		    tp->ac = 1;
            } else {
		    tp->ac = 0;
            }
	    break;

       case MOVM:
       movm:
/*
	    printf("&protodata[pcdata]=%x", &protodata[pcdata]);
	    printf("pcdata=%x", pcdata);
*/
/*
	    chkit();
*/
	    protodata[pcdata] = tp->ac;
	    tp->pc++;
	    break;

       case CLR:
/*
	    chkit();
*/
	    protodata[pcdata] = 0;
	    tp->pc++;
	    break;

       case ORM:
/*
	    chkit();
*/
	    tp->ac |= protodata[pcdata];
	    goto movm;

       case ANDM:
/*
	    chkit();
*/
	    tp->ac &= protodata[pcdata];
	    goto movm;

       case XORM:
/*
	    chkit();
*/
	    tp->ac ^= protodata[pcdata];
	    goto movm;

       case INC:
/*
	    chkit();
*/
	    tp->ac = ++protodata[pcdata];
	    goto movm;

       case DEC:
/*
	    chkit();
*/
	    tp->ac = --protodata[pcdata];
	    goto movm;

       case BICM:
/*
	    chkit();
*/
	    tp->ac &= ~protodata[pcdata];
	    goto movm;

       case ADDM:
/*
	    chkit();
*/
	    tp->ac += protodata[pcdata];
	    goto movm;

       case SUBM:
/*
	    chkit();
*/
	    tp->ac -= protodata[pcdata];
	    goto movm;

       case LSHM:
/*
	    chkit();
*/
	    tp->ac = protodata[pcdata] << tp->ac;
	    goto movm;

       case RSHM:
/*
	    chkit();
*/
	    tp->ac = protodata[pcdata] >> tp->ac;
	    goto movm;

       case TESTOP:
/*
	    chkit();
*/
	    temp = protodata[pcdata];
            tp->ac = temp&01    ^ temp>>1&01 ^ temp>>2&01 ^ temp>>3&01 ^ 
		     temp>>4&01 ^ temp>>5&01 ^ temp>>6&01 ^ temp>>7&01;
	    tp->pc++;
	    break;

       case CRCLOC:
/*
	    chkit();
*/
	    protodata[pcdata] = 0;
	    protodata[pcdata+1] = 0;
	    tp->crcaddr = pcdata;
	    tp->pc++;
	    break;

       case CRC16:
	    /* XOR the stored crc low byte with the passed byte */
/*
	    chkit();
*/
	    tp->ac=(protodata[(tp->crcaddr) & 0xff] ^ protodata[pcdata]) & 0xff;

       crc16: 
		/* store new crc low byte      */
		protodata[(tp->crcaddr) & 0xff]=
		  protodata[(tp->crcaddr+1) & 0xff] ^ bcctab1[(tp->ac) & 0xff];

		/* store new crc high byte     */
		protodata[(tp->crcaddr+1) & 0xff] = bcctab2[(tp->ac) & 0xff];

		tp->pc++;
		break;

       case CRC16I:
	    /* XOR the stored crc low byte with the passed immediate byte */
	    tp->ac=(protodata[(tp->crcaddr) & 0xff] ^ protoinst[tp->pc]) & 0xff;
	    goto crc16;

	case RCV:
#ifndef ASYNC
	getnext:
#endif
/*
		chkit();
*/
		if(rcv(tp,&protodata[pcdata])) {
			tp->pc++;
		} else {
			tp->pc--;
			if(tp->v_iflag & 02) {
				break;
			}
/*
			printf("RCV:NO CHRS");
*/
			return;
		}
#ifndef ASYNC
		if(tp->v_iflag & 01) {
			break;
		}
		if((protodata[pcdata] & 0xff) == (tp->synchar & 0xff)) {
			tp->pc--;
			goto getnext;
		}
		tp->v_iflag |= 01;
#endif
		break;

       case XMT:
/*
	    printf("XMT");
*/
/*
	    chkit();
*/
	    if(xmt(tp,&protodata[pcdata])) {
		    tp->pc++;
            } else {
		    tp->pc--;
		    return;
            }
	    break;

       case XMTI:
/*
	    printf("XMTI");
*/
	    if(xmt(tp,&protoinst[tp->pc])) {
		    tp->pc++;
            } else {
		    tp->pc--;
		    return;
            }
	    break;

       case XSOMI:
/*
	    printf("XSOMI");
*/
	    for(temp=0; temp<6; temp++) {
	         xmt(tp, &protoinst[tp->pc]);
	    }
	    tp->pc++;
	    break;

       case XEOMI:
/*
	    printf("XEOMI");
*/
	    if(xeom(tp,protoinst[tp->pc])) {
		    tp->pc++;
            } else {
		    tp->pc--;
		    return;
            }
	    break;

       case RSOMI:

		tp->v_iflag &= 0xfe;

		tp->synchar = protoinst[tp->pc];
		setrsom(tp);
		tp->pc++;
		break;

       case GETXBUF:

	    tp->pc++;
/*
	    chkit();
*/
	    if(getxbuf(tp,&protodata[pcdata])) {
		    tp->ac = 1;
		    return;
            } else {
		    tp->ac = 0;
            }
	    break;

       case GETRBUF:

	    tp->pc++;
/*
	    chkit();
*/
	    if(getrbuf(tp,&protodata[pcdata])) {
		    tp->ac = 1;
		    return;
            } else {
		    tp->ac = 0;
            }
	    break;

       case GETBYTE:

	    tp->pc++;
/*
	    chkit();
*/
	    if(get(tp,&protodata[pcdata])) {
		    tp->ac = 0;
            } else {
		    tp->ac = 1;
		    return;
            }
	    break;

       case PUTBYTE:

	    tp->pc++;
/*
	    chkit();
*/
	    if(put(tp,&protodata[pcdata])) {
		    tp->ac = 1;
		    return;
            } else {
		    tp->ac = 0;
            }
	    break;

       case PUTBYTEI:

	    if(put(tp,&protoinst[tp->pc++])) {
		    tp->ac = 1;
		    return;
            } else {
		    tp->ac = 0;
            }
	    break;

       case TRACE1:
/*
	    chkit();
*/
	    array[0] = protodata[pcdata];
       trace:
	    array[1] = ((tp->pc) -1) >> 8;
	    array[2] = ((tp->pc) -1);
	    array[3] = 0;

	    if(mreport(VPMTRC, tp->t_dev, array, MTRCEL)) {
		    tp->pc--;
		    return;
            } else {
	            tp->pc++;
		    break;
	    }

       case TRACE1I:

	    array[0] = protoinst[tp->pc];
	    goto trace;

       case TRACE2:
/*
	    chkit();
*/
	    array[0] = protodata[pcdata];
	    array[1] = protodata[pcdata+1];
       trace2:
	    array[2] = ((tp->pc) -1) >> 8;
	    array[3] = ((tp->pc) -1);

	    if(mreport(VPMTRC, tp->t_dev, array, MTRCEL)) {
		    tp->pc--;
		    return;
            } else {
	            tp->pc++;
	            tp->pc++;
		    break;
	    }

       case TRACE2I:

	    array[0] = protoinst[tp->pc];
	    array[1] = protoinst[(tp->pc) +1];
	    goto trace2;

       case SNAP:
/*
	    chkit();
*/
	    if(mreport(VPMSNAP, tp->t_dev, protodata[pcdata], MSNAPL)) {
		    tp->pc--;
		    return;
            } else {
	            tp->pc++;
		    break;
	    }

       case ATOE:
/*
	    chkit();
*/
	    if(temp=atoetbl[protodata[pcdata] & 0xff] & 0xff)
			tp->ac = temp;
	    tp->pc++;
	    break;

       case ETOA:
/*
	    chkit();
*/
	    tp->ac = etoatbl[protodata[pcdata] & 0xff];
	    tp->pc++;
	    break;

       case ATOEI:

	    if(temp=atoetbl[protoinst[tp->pc] & 0xff] & 0xff)
		 tp->ac = temp;
	    tp->pc++;
	    break;

       case ETOAI:

	    tp->ac = etoatbl[protoinst[tp->pc] & 0xff] & 0xff;
	    tp->pc++;
	    break;

       case RTNXBUF:
	    
		if(tp->p_xeomstate)
			goto xmreport;
/*
		chkit();
*/
		rtnxbuf(tp,&protodata[pcdata]);

		/*  send a rtnxbuf signal to the unix driver here */

	xmreport:
/*
		chkit();
*/
		if(mreport(VPMRTNX, tp->t_dev, protodata[pcdata], MRTNXL)) {
			tp->p_xeomstate = 1;
			tp->pc--;
			return;
		} else {
			tp->p_xeomstate = 0;
			tp->pc++;
			break;
		}

       case RTNRBUF:
	    
		if(tp->t_canq.c_cc != 0) {
			tp->pc--;
			return;
		}
		if(onlyone) {
			return;
		}
/*
		chkit();
*/
		rtnrbuf(tp,&protodata[pcdata]);

		tp->pc++;
		break;

       case DSRWAIT:

	    if(tp->t_state & CARR_ON) {
	            tp->pc++;
	            break;
            } else {
		    tp->pc--;
		    return;
            }

       case GETCMD:
	    if(tp->t_state & ISACMD) {
		 for(temp=0; temp<4; temp++) {
/*
		      chkit();
*/
		      protodata[pcdata + temp] = tp->v_cmdar[temp];
                 }
		 tp->ac = 1;
		 tp->t_state &= ~ISACMD;

            } else {
		    tp->ac = 0;
            }
	    tp->pc++;
	    break;

       case RTNRPT:
/*
	    chkit();
*/
	    if(mreport(VPMRPTI, tp->t_dev, protodata[pcdata], MREPTL)) {
		    tp->pc--;
		    return;
            } else {
	            tp->pc++;
		    break;
	    }

       case TIMEOUTI:

	    /* TIMEOUT:  if(Y==0) then cancel any previous time-out request; */
	    /*           else {                                              */
	    /*                 save stack pointer;                           */
	    /*                 save the address of the next instruction      */
	    /*                 set timer = Y;                                */
	    /*                 set AC = 0;                                   */
	    /*           }                                                   */
	    /*           When the timer expires, restore the saved stack     */
	    /*           pointer, set AC = 1, and set the program counter    */
	    /*           equal to the saved address from above.              */
	    /*                                                               */
	    /*           A return from the stack frame(i.e. CALL and RETURN) */
	    /*           which was active at the time thr time-out request   */
	    /*           was made will cancel the time-out request.          */
	    /*                                                               */
	    /* Note:  The timeout parameter passed via the AC is the number  */
	    /*        of 100-millisecond increments until the timeout occurs */

	    /* get the passed timeout value or zero */
	    temp = protoinst[tp->pc++];

	    /* Check to see if a timeout request is currently running */
	    /* If it is and the value of the passed parameter is not  */
	    /* zero, cancel the currently running timeout.            */
/*
	    if(tp->v_iflag & 02) {
		break;
	    }
*/
	    s = spl6();
	    if(tp->v_timerid && temp) {
		 cantimer(tp->v_timerid);
            }
	    splx( s );

	    if(tp->stacktop != tp->stacknext) {

		 /* there is an active stack entry */
                 tp->stackactive = tp->stacknext;
            }
	    /* get a unique timer id */
	    tp->v_timerid = timerid;
	    timerid++;

	    /* save the program counter following the timeout instruction */
	    tp->v_timertpc = tp->pc;

	    if(temp) {
		      s = spl6();
		      timeout(timertn, tp, temp*5, tp->v_timerid);
            } else {
		    s = spl6();
		    cantimer(tp->v_timerid);
		    tp->v_timerid = 0;

		    tp->stackactive = 0;

		    tp->v_timertpc = 0;

            }
	    tp->ac = 0;
	    splx( s );
	    break;

       case TIMERI:

	    /* timer(n) - if n>0 then reset the timer to expire in n*100 */
	    /*            milliseconds and return(1); else if the timer  */
	    /*            has not expired then return the current value  */
	    /*            of the timer; else return(0).                  */

            temp = protoinst[tp->pc++];
	    s = spl6();
	    if(temp == 0) {
		    tp->ac = timeval(tp->v_tim2id);
            } else {
	            /* check to see if a timer request */
		    /* is currently running.           */

	            if(tp->v_tim2id) {
			 cantimer(tp->v_tim2id);
		    }
		    tp->v_tim2id = timerid;
		    timerid++;
		    timeout(tim2func, tp, temp*5, tp->v_tim2id);
		    tp->ac = 1;
            }
	    splx( s );
	    break;

       case RETURN:
/*
	    chkit();
*/
	    tp->ac = protodata[pcdata];

	    tp->pc++;

       popit:
/*
	    printf("RETURN");
*/
		s = spl6();
		if(tp->v_iflag & 02) {
			tp->pc--;
			tp->pc--;
			splx( s );
			break;
		}
		if(tp->stacktop == tp->stacknext) {
			goto stackerr;
		} else {
			/* The following logic deals with timer cancel   */
			/* if a timeout was set within a CALL and RETURN */

			if((tp->stackactive != 0) &&
					(tp->stackactive == tp->stacknext)) {
				cantimer(tp->v_timerid);

				tp->v_timerid = 0;
			}
			tp->stacknext++;
			tp->stacknext++;

			tp->pc = ((protodata[tp->stacknext-2] & 0xff) |
				(protodata[tp->stacknext-1] * 256) & 0x1fff);
/*
	    if(tp->ac == 5 || tp->ac == 011 || tp->ac == 012 || tp->ac == 017) {
			printf("RETURN:PC=%x, AC=%x", tp->pc, tp->ac);
	    }
*/
			tp->pc++;
			tp->pc++;
			if(tp->stacktop == tp->stacknext) {
				tp->stacktop = sstack;
				tp->stacknext = sstack;
				}
			}
		splx( s );
		break;

       case RETURNI:
		 
            tp->ac = protoinst[tp->pc++];
/*
	    printf("RETI:AC=%x", tp->ac);
*/
	    goto popit;

       case HALT:
/*
	    chkit();
*/
	    array[2] = protodata[pcdata];
       halt:
	    array[3] = EXITINST;
       errterm:
	    array[0] = ((tp->pc) -1) >> 8 & 0xff;
	    array[1] = ((tp->pc) -1) & 0xff;

	    if(tp->v_timerid) {
		 cantimer(tp->v_timerid);
		 tp->v_timerid = 0;
            }

	    if(mreport(VPMERT, tp->t_dev, array, MERTL)) {
	            tp->pc--;
	            return;
	    } else {
	            devicearray[TDEV(tp->t_dev)] = 4;
                    return;
            }
       case HALTI:

	    array[2] = protoinst[tp->pc];
	    goto halt;

       default:
	    array[2] = 0;
	    array[3] = ILLEGAL;
	    goto errterm;

     }
     goto doagain;

     /*  This is the logic to handle the jumps */

     jmpinst:

       temp = protoinst[tp->pc] & 0260;	/* jump op code */
       temp1 = protoinst[tp->pc] & 017;	/* page no. */
       temp1 = (temp1 * 256) & 0x1fff;	/* no. of pages times 256 */
/*
       printf("fetch/jmp: temp= %x\n", temp);
       printf("fetch/jmp: temp1= %x\n", temp1);
*/
       if(protoinst[(tp->pc) +1]&01 || temp1&01)
	    goto jumperr;

       switch (temp) {

	 case JMP:
         jump:
	      tp->pc++;
/*
	      printf("JMP:temp1=%x", temp1);
*/
	      tp->pc = (protoinst[tp->pc] & 0xff) | temp1;
/*
	      printf("JMP TO %x", tp->pc);
*/
	      break;

         case JMPEQZ:
	      tp->pc++;
/*
	      printf("fetch/JMPEQZ: tp->ac= %x\n", tp->ac);
*/
	      if(tp->ac == 0) {
		      tp->pc = (protoinst[tp->pc] & 0xff) | temp1;
              } else {
                      tp->pc++;
              }
/*
	      printf("JMPEQZ TO %x", tp->pc);
*/
	      break;

	 case JMPNEZ:
	      tp->pc++;
/*
	      printf("fetch/JMPNEZ: tp->ac= %x\n", tp->ac);
*/
	      if(tp->ac != 0) {
		      tp->pc = (protoinst[tp->pc] & 0xff) | temp1;
              } else {
                      tp->pc++;
              }
/*
	      printf("JMPNEZ TO %x", tp->pc);
*/
	      break;

         case CALL:
/*
		printf("CALL %x", protoinst[tp->pc+1]);
*/
		tp->stacknext--;
		tp->stacknext--;

		if(tp->stacknext <= (sstack-30))
			goto stackerr;

		/* CURRENT PAGE = CURRENT PC divided by 256 */

		protodata[tp->stacknext+1] = ((tp->pc)/256) & 017;

		/* PC = CURRENT PC minus CURRENT PAGE times 256 */

		protodata[tp->stacknext] = 
			(tp->pc - (protodata[tp->stacknext+1] * 256)) & 0xff;
		
		goto jump;

       }
istimout:
	s = spl6();
	if(tp->v_iflag & 02) {

			tp->v_iflag = 0;
			tp->pc = tp->v_timertpc;
			tp->ac = 1;

			tp->stacknext = savestkactv;

			tp->v_timerid = 0;
/*
		printf("fetch:timeout:pc=%x, v_iflag=%x", tp->pc, tp->v_iflag);
*/
		splx( s );
		goto doagain;
	}
	splx( s );
	if(++numtimes > 100) {
		return;
	} else {
		goto doagain;
	}
jumperr:
       array[2] = 0;
       array[3] = JMPERR;
       goto errterm;

stackerr:
       array[2] = 0;
       array[3] = STACKERR;
       goto errterm;

fetcherr:
       array[2] = 0;
       array[3] = FETCHERR;
       goto errterm;

}
