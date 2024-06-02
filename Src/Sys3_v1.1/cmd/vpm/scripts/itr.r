include(const)
include(tconst)
define(nextfrmsent,	{nextkind = $2})
define(incmod,	{++$1; if($1>=8)$1 = 0})
define(submod,	{$3 = $1-$2; if($1 < $2)$3 += 8})
array ac[5];
function lapb()
	nextkind = NONE;
	state = INFO_TR;
	stateI = NRSQ_BTHI;
	timer(TA);
	rsxmtq();
	repeat{
		rcvc = rcvp();
		switch(rcvc){
		case NONE_r:
		case Iok_r:
			switch(stateI){
			case NRSQ_BTHI:
				nextfrmsent(SUP,RR);
			case SQER_BTHI:
				nextfrmsent(SUP,RR);
				stateI = NRSQ_BTHI;
				n2 = 0;
			}
			rtnrfrm();
			incmod(VR);
		case Inok_r:
			switch(stateI){
			case NRSQ_BTHI:
				nextfrmsent(SUP,REJ);
				stateI = SQER_BTHI;
				n2 = 0;
			}
		case REJ_r:
			if(inh == 2) if(rcvNR == checkVS) break;
			VS = rcvNR;
			rxmt = 1;
		case NBUF_r:
			trace(LOCAL_BUSY);
		case OVRRN_r:
			trace(LOCAL_BUSY);
		case INVLD_r:
			trace(INVLD_r);
			trace(err);
		case BADCRC_r:
			trace(BADCRC_r);
		case DSCRD_r:
		}
		if(xmtbusy() == 0)
			xmtp();
	}
end
function rcvp()
	rcvf = rcvfrm(ac);
	if(rcvf==0)
		return(NONE_r);
	if(rcvf&020){
		rcvf &= 017;
		if(rcvf>4)
			return(NONE_r);
	}
		if(t1 == 0) timer(TA);
		if(rcvf < 4)	return(BADCRC_r);
		addr = ac[0];
		switch(addr){
#		ADDRESSES SET UP FOR LOOP AROUND ONLY
			case B:
				if(Aaddr) rcvdisp = RESP; else rcvdisp = COMM;
			case A:
				if(Aaddr) rcvdisp = COMM; else rcvdisp = RESP;
			default:
				return(BADCRC_r);
		}
		ctrl = ac[1];
		if(ctrl&BIT5)
			if(rcvdisp == COMM){
				P1r = 1;
				F1r = 0;
				}
			else
				if(PFcycle){
					F1r = 1;
					if(state == INFO_TR){
						PFcycle = 0;
						Pbit = 0;
						inh = 0;
					}
				}
				else{
					err = UNSOL_F1;
					return(BADCRC_r);
				}
		else F1r = 0;
		if((ctrl&BIT1) == 0){
			if(state == INFO_TR){
#	I-frame
				if(rcvf == 4)
					return(BADCRC_r);
#		null I-field
				rcvNR = (ctrl&BIT678)>>5;
				validNR();
				rtnbufs();
				if(F1r)
					checkpt();
				rcvNS = (ctrl&BIT234)>>1;
				if(rcvNS == VR)
					return(Iok_r);
				else
					return(Inok_r);
			}
		}
		else{
			if((ctrl&BIT2) == 0){
				if(state == INFO_TR){
#	SUP-frame
					if(rcvf != 4){
						err = INCLGTH_X;
						return(INVLD_r);
					}
					rcvNR = (ctrl&BIT678)>>5;
					validNR();
					rtnbufs();
					type = ctrl&BIT34;
					switch(type){
					case BREJ:
						if(PFcycle){if(inh==0)inh=1;}
						else inh = 0;
						return(REJ_r);
					case BRR:
						if(F1r)
							checkpt();
						return(RR_r);
					}
				}
			}
		}
end
function validNR()
	submod(rcvNR,ackVS,newack);
	submod(VS,ackVS,pendack);
	if(newack > pendack){
		err = BADNR;
		return(INVLD_r);
	}
end
function checkpt()
	if(checkVS != rcvNR){
		VS = rcvNR;
		rxmt = 1;
		inh = 2;
	}
end
function rtnbufs()
	if(newack){
		s = ackVS;
		while(s != rcvNR){
			rtnxfrm(s);
			incmod(s);
		}
		ackVS = rcvNR;
		if(stateI == NRSQ_BTHI) n2 = 0;
		if(VS != rcvNR) t1 = timer(T1);
		else
			switch(stateI){
			case NRSQ_BTHI:
				if(PFcycle == 0){
					timer(TA); t1 = 0;
				}
			}
	}
end
function xmtp()
		if(P1r){
			switch(state){
			case INFO_TR:
				trace(Pir);
				if(nextkind == NONE)
					nextfrmsent(SUP,RR);
			}
		}
		else{
			if(timer(0) == 0){
				PFcycle = 1;
				Pbit = 0;
				inh = 0;
			}
			if(PFcycle) if(Pbit == 0){
				if(timer(0) == 0){
					++n2;
					if(n2 == N2){
						trace(RETR_EXCD);
					}
				}
				t1 = timer(T1);
				switch(state){
				case INFO_TR:
					checkVS = VS;
					switch(stateI){
					case SQER_BTHI:
						nextfrmsent(SUP,REJ);
					case NRSQ_BTHI:
						nextfrmsent(SUP,RR);
					}
				}
			goto break1;
			}
			switch(nextkind){
			case RR:
				goto csNONE;
			case NONE:
				csNONE:
				switch(stateI){
				case NRSQ_BTHI:
					goto csSEBI;
				case SQER_BTHI:
					csSEBI:
					submod(VS,ackVS,pendack);
					if(VS != lastVS)
						{nextfrmsent(INFO,I);}
					else
					if(pendack <= WINDOW)
					 if(getxfrm(VS) == 0){
						trace(VS);
						{nextfrmsent(INFO,I);}
					}
				}
			}
		}
		break1:
		if(nextkind != NONE)
			buildfrm();
end
function buildfrm()
	if(Aaddr){
		addrCOMM = B;
		addrRESP = A;
	}
	else{
		addrCOMM = A;
		addrRESP = B;
	}
	trace(nextkind);
	if(nextkind == I){
		ac[0] = addrCOMM;
		ac[1] = ((VR<<4)|VS)<<1;
		if(rxmt){
			rxmt = 0;
			if(PFcycle == 0)
				PFcycle = 1;
		}
		if(t1 == 0)
			t1 = timer(T1);
	}
	else{
			if(PFcycle) if(Pbit == 0){
				ac[0] = addrCOMM;
				ctrl = BIT5;
				Pbit = 1;
				t1 = timer(T1);
				goto break2;
			}
			ac[0] = addrRESP;
			if(P1r){
				ctrl = BIT5;
				P1r = 0;
			}
			else
				ctrl = 0;
			break2:
			ctrl |= VR<<5;
			switch(nextkind){
			case RR:
				ctrl |= BIT1;
			case REJ:
				ctrl |= BIT14;
				t1 = timer(T1);
			}
			ac[1] = ctrl;
	}
	setctl(ac,2);
	if(nextkind == I){
		if(ctI == CTI){
			ctI = 0;
			trace(I);
			trace(255);
			trace(255);
			goto skp1;
		}
		if(xmtfrm(VS) == 0){
			skp1:	++ctI;
			if(lastVS == VS)
				{incmod(lastVS);}
			trace(I); trace(VS); trace(VR);
			incmod(VS);
		}
	}
	else{
		switch(nextkind){
		case RR:
			if(ctRR == CTRR){
				ctRR = 0;
				trace(RR);
				trace(255);
				trace(255);
				goto skp2;
			}
			++ctRR;
		case REJ:
			if(ctREJ == CTREJ){
				ctREJ = 0;
				trace(REJ);
				trace(255);
				trace(255);
				goto skp2;
			}
			++ctREJ;
		}
		xmtctl();
		trace(nextkind); trace(PFcycle); trace(VR);
	}
	skp2:
	nextkind = NONE;
end
