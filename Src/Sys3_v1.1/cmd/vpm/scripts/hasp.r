define(do,repeat)
define(INIT,1)
define(SENTACK,2)
define(SENTBLK,3)
define(DLE,0x10)
define(ENQ,0x2d)
define(ETB,0x26)
define(SOH,0x01)
define(NAK,0x3d)
define(NULL,0x00)
define(PAD,0xff)
define(X70,0x70)
define(STX,0x02)
define(SYNC,0x32)
define(CKSEQ,0x80)
define(NOCK,0x90)
define(RSSEQ,0xa0)
define(WAITBIT,0x40)
define(INITFLT,5)
define(RETRY,06)
define(TIMEOUT,010)
define(R_ACK0,1)
define(R_ACK1,2)
define(R_ENQ,3)
define(R_ERBLK,5)
define(R_NAK,6)
define(R_OKBLK,7)
define(R_SEQER,10)
define(ERR1,011)
define(ERR2,012)
define(ERR3,013)
define(ERR4,014)
define(xmtctl,	{c1 = $1; c2 = $2;	xmtctl1()})
define(xmtcrc,	{crc16($1); xmt($1)})
define(incmod,	{++$1; if($1 >= $2)$1 = 0})
define(rcvtr,	{rcv($1); trace($1)})
array crc[2];
array rtype[3];
array xtype[3];
function hasp()
	state = INIT;
	xbcb = 0xa0;
	xfcs0 = 0x8f;
	xfcs1 = 0xcf;
	xmtctl(SOH,ENQ);
	trace(0100);
	do{
		trace(state);
		rcvs = rec();
		trace(rcvs);
		switch(state){
		case INIT:
			switch(rcvs){
			case R_ACK0:
				trace(0101);
				xmtrsp();
			default:
				if(ct0<30){
					++ct0;
					xmtctl(SOH,ENQ);
					trace(0102);
					state = INIT;
				}else{
					trace(0112);
					exit(INITFLT);
				}
			}
		case SENTACK:
			switch(rcvs){
			case R_ACK0:
				ct1 = 0;
				xmtrsp();
			case R_OKBLK:
				ct1 = 0;
				rseqck();
			case R_ERBLK:
				ct1 = 0;
				xmtctl(NAK,NULL);
				state = SENTACK;
			case R_NAK:
				ct1 = 0;
				xmtctl(DLE,X70);
				state = SENTACK;
			default:
				++ct1;
				if(ct1 >= 6){
					exit(RETRY);
				}else{
					trace(0120);
					xmtctl(NAK,NULL);
					state = SENTACK;
				}
			}
		case SENTBLK:
			switch(rcvs){
			case R_ACK0:
				ct1 = 0;
				xbcb = 0x80|xseq;
				incmod(xseq,16);
				rtnxbuf(xtype);
				xmtrsp();
			case R_OKBLK:
				ct1 = 0;
				xbcb = 0x80|xseq;
				incmod(xseq,16);
				rtnxbuf(xtype);
				rseqck();
			case R_ERBLK:
				ct1 = 0;
				xmtctl(NAK,NULL);
				state = SENTBLK;
			case R_NAK:
				ct1 = 0;
				getxbuf(xtype);
				xmtblk();
				state = SENTBLK;
			default:
				++ct1;
				if(ct1 >= 6){
					exit(RETRY);
				}else{
					trace(0122);
					xmtctl(NAK,NULL);
					state = SENTBLK;
				}
			}
		}
	}
end
function xmtblk()
	crcloc(crc);
	trace(0105);
	trace(xbcb);
	xsom(SYNC);
	xmt(DLE);
	xmt(STX);
	xmtcrc(xbcb);
	xmtcrc(xfcs0);
	xmtcrc(xfcs1);
	ct2 = 0;
	while(get(byte) == 0){
		++ct2;
		if(byte == DLE)
			xmt(DLE);
		xmtcrc(byte);
	}
	xmt(DLE);
	xmtcrc(ETB);
	xmt(crc[0]);
	xmt(crc[1]);
	xeom(PAD);
	trace(ct2);
end
function xmtctl1()
	xsom(SYNC);
	xmt(c1);
	if(c2 != NULL)
		xmt(c2);
	xeom(PAD);
end
function rec()
	if(timeout(100))
		return(TIMEOUT);
	ctn = 0;
	crcloc(crc);
	rsom(SYNC);
	rcv(byte);
	switch(byte){
	case NAK:
		trace(byte);
		rcv(byte);
		byte &= 017;
		if(byte == 017)
			return(R_NAK);
		else
			return(ERR1);
	case DLE:
		rcv(byte);
		switch(byte){
		case X70:
			rcv(byte);
			byte &= 017;
			if(byte == 017)
				return(R_ACK0);
			else
				return(ERR1);
		case STX:
			while(getrbuf(rtype));
			rcvtr(rbcb);
			crc16(rbcb);
			rcvtr(rfcs0);
			crc16(rfcs0);
			rcvtr(rfcs1);
			crc16(rfcs1);
			do{
				rcv(byte);
				if(byte != DLE){
					put(byte);
					crc16(byte);
					++ctn;
				}else{
					rcv(byte);
					switch(byte){
					case DLE:
						put(byte);
						crc16(byte);
						++ctn;
					case SYNC:
					case ETB:
						break;
					default:
						return(ERR2);
					}
				}
			}
				trace(ctn);
				crc16(byte);
				rcv(byte);
				crc16(byte);
				rcv(byte);
				crc16(byte);
				rcv(byte);
				byte &= 017;
				if(byte != 017)
					return(ERR1);
				if(crc[0] != 0)
					return(R_ERBLK);
				if(crc[1] != 0)
					return(R_ERBLK);
				return(R_OKBLK);
		default:
			trace(byte);
			return(ERR3);
		}
	default:
		trace(byte);
		return(ERR4);
	}
end
function xmtserr()
	xsom(SYNC);
	xmt(DLE);
	xmt(STX);
	xmtcrc(0x90);
	xmtcrc(xfcs0);
	xmtcrc(xfcs1);
	xmtcrc(0xf0);
	xmtcrc(0x80);
	xmtcrc(0);
	xmtcrc(0);
	xmt(DLE);
	xmtcrc(ETB);
	xmt(crc[0])
	xmt(crc[1]);
	xeom(PAD);
end
function xmtrsp()
	if(wait){
		xmtctl(DLE,X70);
		state = SENTACK;
		return;
	}
	if(getxbuf(xtype)){
		xmtctl(DLE,X70);
		state = SENTACK;
	}else{
		xmtblk();
		trace(0104);
		state = SENTBLK;
	}
end
function rseqck()
	trace(rseq);
	if(rfcs0&WAITBIT){
		wait = 1;
		trace(rfcs0);
	}else{
		wait = 0;
	}
	switch(rbcb&0xf0){
		case CKSEQ:
			s = rbcb&017;
			if(s == rseq){
				incmod(rseq,16);
				rtnrbuf(rtype);
				xmtrsp();
			}else{
				if((rseq-s) > 3){
					xmtserr();
					state = SENTBLK;
				}else{
					xmtctl(DLE,X70);
					state = SENTACK;
				}
			}
		case NOCK:
			rtnrbuf(rtype);
			xmtctl(DLE,X70);
			state = SENTACK;
			trace(0110);
			return;
		case RSSEQ:
			rseq = rbcb&0x0f;
			rtnrbuf(rtype);
	}
end
