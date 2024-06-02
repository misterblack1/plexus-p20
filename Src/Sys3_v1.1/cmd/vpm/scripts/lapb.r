include(const)
include(tconst)
define(getcmd,0)
define(nextfrmsent,	{nexttype = $1; nextkind = $2})
define(incmod,	{++$1; if($1 >= 8)$1 = 0})
define(submod,	{$3 = $1-$2; if($1 < $2)$3 += 8})
array ac[5];
array parm[4];
array stat[4];
#	SUBJECT TO CHANGE: QUESTIONS TO R. ERMANN  x2837
#	TRANSMISSION AND RECEPTION IN THE ORDER OF INCREASING BIT
#		SIGNIFICANCE IS ASSUMED.
#	NOT TO BE DISCLOSED WITHOUT PERMISSION.
function lapb()
	nextfrmsent(UNN,SABM);
	state = SABM_t;
	Aadr = 1;
	repeat{
		rcvc = rcvp();
		switch(rcvc){
		case NONE_r:
		case Iok_r:
			switch(stateI){
			case NRSQ_REMI:
				nextfrmsent(SUP,RR);
			case SQER_REMI:
				nextfrmsent(SUP,RR);
				stateI = NRSQ_REMI;
				n2 = 0;
			default:
				nextfrmsent(SUP,RR);
				if(newack){
					stateI = NRSQ_REMI;
					n2 = 0;
				}
				else stateI = NRSQ_REMB;
			}
			rtnrfrm();
			incmod(VR);
		case Inok_r:
			switch(stateI){
			case NRSQ_REMI:
				nextfrmsent(SUP,REJ);
				stateI = SQER_REMI;
				n2 = 0;
			case NRSQ_REMB:
				nextfrmsent(SUP,REJ);
				if(newack)
					stateI = SQER_REMI;
				else stateI = SQER_REMB;
			case SQER_REMB:
				if(newack)
					stateI = SQER_REMI;
			}
		case RR_r:
			switch(stateI){
			case NRSQ_REMB:
				stateI = NRSQ_REMI;
				n2 = 0;
			case SQER_REMB:
				stateI = SQER_REMI;
			}
		case REJ_r:
			switch(stateI){
			case NRSQ_REMB:
				stateI = NRSQ_REMI;
				n2 = 0;
			case SQER_REMB:
				stateI = SQER_REMI;
			}
			if(inh == 2) if(rcvNR == checkVS) break;
			VS = rcvNR;
			rxmt = 1;
			if(xmtbusy())if(sentI) abtxfrm();
		case RNR_r:
			switch(stateI){
			case NRSQ_REMI:
				stateI = NRSQ_REMB;
				n2 = 0;
			case SQER_REMI:
				stateI = SQER_REMB;
			}
		case SABM_r:
			trace(INCOMING_CALL);
			switch(state){
			case DISC_t:
				nextfrmsent(UNN,DM);
			case SABM_t:
				nextfrmsent(UNN,UA);
			default:
				nextfrmsent(UNN,UA);
				state = INFO_TR;
				stateI = NRSQ_REMI;
				timer(TA); t1 = 0; n2 = 0;
				VS = 0; ackVS = 0; lastVS = 0; VR = 0;
				PFcycle = 0; Pbit = 0;
				rsxmtq();
			}
		case DISC_r:
			trace(CLEAR_INDICATION);
			switch(state){
			case DISC_PH:
				nextfrmsent(UNN,DM);
			case DISC_t:
				nextfrmsent(UNN,UA);
			default:
				nextfrmsent(UNN,UA);
				state = DISC_PH;
			}
		case UA_r:
			trace(COMMAND_ACCEPTED);
			switch(state){
			case SABM_t:
				state = INFO_TR;
				stateI = NRSQ_REMI;
				timer(TA); t1 = 0; n2 = 0;
				VS = 0; ackVS = 0; lastVS = 0; VR = 0;
				PFcycle = 0; Pbit = 0;
				rsxmtq();
			case DISC_t:
				state = DISC_PH;
			case INFO_TR:
				trace(UNSOLIC_UA);
				nextfrmsent(UNN,SABM);
				state = SABM_t;
				n2 = 0;
			}
		case DM_r:
			if(F1r)
				trace(COMMAND_NOT_ACC);
			else
				trace(MODE_SET_SOLICITED);
			switch(state){
			case FRMR_t:
				goto csINTR;
			case DM_t:
				goto csINTR;
			case INFO_TR:
				csINTR:
				nextfrmsent(UNN,SABM);
				state = SABM_t;
				n2 = 0;
			default:
				state = DISC_PH;
			}
		case FRMR_r:
			trace(FRMR_RCVD);
			switch(state){
			case INFO_TR:
				goto csFRMRt;
			case DM_t:
				goto csFRMRt;
			case FRMR_t:
				csFRMRt:
				nextfrmsent(UNN,SABM);
				state = SABM_t;
				n2 = 0;
			}
		case INVLD_r:
			if(state == INFO_TR){
				nextfrmsent(UNN,FRMR);
				state = FRMR_t;
				n2 = 0; t1 = timer(T1);
				ac[2] = ac[1];
				if(rcvdisp == COMM)
					ctrl = BIT5;
				else
					ctrl = 0;
			}
		case NBUF_r:
			goto csOVRRN;
		case OVRRN_r:
			csOVRRN:
			trace(LOCAL_BUSY);
			if(lbusy == 0){
				lbusy = 1;
				nextfrmsent(SUP,REJ);
			}
		case DSCRD_r:
		}
		if(xmtbusy() == 0)
			xmtp();
	}
end
function rcvp()
	rcvf = rcvfrm(ac);
	if(rcvf > 0){
		if(ct>IERR){
			trace(ac[1]);
			rcvf = 0;
			ct = 0;
		}
		++ct;
	}
	{
	if(rcvf==0)
		return(NONE_r);
	if(rcvf&020){
		rcvf &= 017;
		if(rcvf>4)
			return(NONE_r);
	}
		if(t1 == 0) timer(TA);
		if(lbusy){
			lbusy = 0;
			nextfrmsent(SUP,RR);
		}
		if(rcvf < 4)	return(DSCRD_r);
		addr = ac[0];
		switch(addr){
			case B:
				if(Aadr) rcvdisp = RESP; else rcvdisp = COMM;
			case A:
				if(Aadr) rcvdisp = COMM; else rcvdisp = RESP;
			default:
				return(DSCRD_r);
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
					return(DSCRD_r);
				}
		else F1r = 0;
		if((ctrl&BIT1) == 0){
			if(state == INFO_TR){
#	I-frame
				if(rcvf == 4)
					return(DSCRD_r);
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
						if(PFcycle){if(inh==0) inh=1;}
						else inh = 0;
						return(REJ_r);
					case BRR:
						if(F1r)
							checkpt();
						return(RR_r);
					case BRNR:
						if(F1r)
							checkpt();
						return(RNR_r);
					default:
						err = CTRLF_W;
						return(INVLD_r);
					}
				}
			}
			else{
#	UNN-frame
					if(F1r) if(state != INFO_TR){
						PFcycle = 0;
						Pbit = 0;
						}
					type = ctrl&BIT34678;
					if(type == BFRMR)
						if(rcvf == 7){
							rcvNR=(ac[3]&BIT678)>>5;
							validNR();
							rtnbufs();
							return(FRMR_r);
						}
						else{
							err = INCLGTH_X;
							return(INVLD_r);
						}
					else
						if(rcvf != 4){
							err = INCLGTH_X;
							return(INVLD_r);
							}
					switch(type){
					case BDM:
						if(rcvdisp == COMM){
							Aadr = 1-Aadr;
							return(DSCRD_r);
						}
						return(DM_r);
					case BSABM:
						if(rcvdisp == RESP){
							switch(state){
							case DISC_PH:
							nextfrmsent(UNN,DM);
							case SABM_t:
							nextfrmsent(UNN,DM);
							}
							return(DSCRD_r);
						}
						return(SABM_r);
					case BDISC:
						if(rcvdisp == RESP)
							return(DSCRD_r);
						return(DISC_r);
					case BUA:
						if(rcvdisp == COMM)
							return(DSCRD_r);
						return(UA_r);
					default:
						if(state == INFO_TR){
							err = CTRLF_W;
							return(INVLD_r);
						}else	return(DSCRD_r);
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
		if(xmtbusy())if(sentI) abtxfrm();
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
		if(stateI == NRSQ_REMI) n2 = 0;
		if(VS != rcvNR) t1 = timer(T1);
		else
			switch(stateI){
			case NRSQ_REMI:
				if(PFcycle == 0){
					timer(TA); t1 = 0;
				}
			case NRSQ_REMB:
				if(PFcycle == 0){
					timer(TA); t1 = 0;
				}
			}
	}
end
function xmtp()
	switch(state){
	case INFO_TR:
		cmd = getcmd(parm,4);
	case DISC_PH:
		cmd = getcmd(parm,4);
	case DM_t:
		cmd = getcmd(parm,4);
	default:
		cmd = 0;
	}
	if(cmd == 0){
		if(P1r){
			switch(state){
			case INFO_TR:
				if(nextkind == NONE)
					nextfrmsent(SUP,RR);
			case FRMR_t:
				nextfrmsent(UNN,FRMR);
			case DM_t:
				P1r = 0;
				nextfrmsent(UNN,DM);
			case DISC_PH:
				nextfrmsent(UNN,DM);
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
						switch(state){
						case SABM_t:
							state = DISC_PH;
							n2 = 0;
							return;
						case DISC_t:
							state = DISC_PH;
							n2 = 0;
							return;
						default:
							nextfrmsent(UNN,SABM);
							state = SABM_t;
							n2 = 0;
						}
					}
				}
				t1 = timer(T1);
				switch(state){
				case INFO_TR:
					checkVS = VS;
					switch(stateI){
					case SQER_REMI:
						nextfrmsent(SUP,REJ);
					case NRSQ_REMI:
						nextfrmsent(SUP,RR);
					case NRSQ_REMB:
						nextfrmsent(SUP,RR);
					default:
						nextfrmsent(SUP,REJ);
					}
				case SABM_t:
					nextfrmsent(UNN,SABM);
				case DISC_t:
					nextfrmsent(UNN,DISC);
				case DM_t:
					nextfrmsent(UNN,DM);
				case FRMR_t:
					nextfrmsent(UNN,FRMR);
				}
				goto break1;
			}
			switch(nextkind){
			case RR:
				goto csNONE;
			case NONE:
				csNONE:
				switch(stateI){
				case NRSQ_REMI:
					goto csSEBI;
				case SQER_REMI:
					csSEBI:
					submod(VS,ackVS,pendack);
					if(VS != lastVS)
						{nextfrmsent(INFO,I);}
					else
					if(pendack <= WINDOW)
					 if(getxfrm(VS) == 0)
						{nextfrmsent(INFO,I);}
				}
			}
		}
		break1:
		if(nextkind != NONE)
			buildfrm();
	}
	else{
		switch(parm[0]){
		case BIT123456:
			nextfrmsent(UNN,SABM);
			state = SABM_t;
			buildfrm();
		case BIT1257:
			nextfrmsent(UNN,DISC);
			state = DISC_t;
			buildfrm();
		case BIT1234:
			nextfrmsent(UNN,DM);
			state = DM_t;
			P1r = 0;
			t1 = timer(T1);
			buildfrm();
		}
	}
end
function buildfrm()
	if(Aadr){
		addrCOMM = B;
		addrRESP = A;
	}
	else{
		addrCOMM = A;
		addrRESP = B;
	}
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
	else
		if(nexttype == SUP){
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
				if(lbusy)
					ctrl |= BIT13;
				else
					ctrl |= BIT1;
			case REJ:
				if(lbusy)
					ctrl |= BIT13;
				else{
					ctrl |= BIT14;
					t1 = timer(T1);
				}
			}
			ac[1] = ctrl;
		}
		else{
			switch(nextkind){
			case SABM:
				ac[0] = addrCOMM;
				ac[1] = BIT123456;
				PFcycle = 1; Pbit = 1;
				t1 = timer(T1);
			case DISC:
				ac[0] = addrCOMM;
				ac[1] = BIT1257;
				PFcycle = 1; Pbit = 1;
				t1 = timer(T1);
			case UA:
				ac[0] = addrRESP;
				if(P1r){
					ac[1] = BIT12567;
					P1r = 0;
				}
				else
					ac[1] = BIT1267;
			case DM:
				ac[0] = addrRESP;
				if(P1r){
					ac[1] = BIT12345;
					P1r = 0;
				}
				else
					ac[1] = BIT1234;
			case FRMR:
				ac[0] = addrRESP;
				if(P1r){
					ac[1] = BIT12358;
					P1r = 0;
				}
				else
					ac[1] = BIT1238;
				ctrl |= VR<<5;
				ac[3] = (VS<<1)|ctrl;
				switch(err){
				case CTRLF_W:
					ac[4] = BIT1;
				case INCLGTH_X:
					ac[4] = BIT12;
				case OVRFLW:
					ac[4] = BIT3;
				case BADNR:
					ac[4] = BIT4;
				}
			}
		}
	setctl(ac,2);
	trace(nexttype);
	sentI = 0;
	if(nexttype == INFO){
		if(xmtfrm(VS) == 0){
			if(lastVS == VS)
				{incmod(lastVS);}
			incmod(VS);
			sentI = 1;
		}
	}
	else
		if(nextkind != FRMR)
			xmtctl();
		else
			exit(0105);	# was xmtctl(5)
	nextkind = NONE;
end
