define(do,repeat)
define(INIT,020)
define(SENTACK,021)
define(SENTBLK,022)
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
define(INITFLT,1)
define(ERRORS,2)
define(R_ACK,1)
define(R_WAIT,2)
define(R_ENQ,3)
define(R_ERBLK,5)
define(R_NAK,6)
define(R_OKBLK,7)
define(R_SEQER,010)
define(R_ERROR,011)
define(TIMEOUT,012)
define(X_ENQ,013)
define(X_NAK,014)
define(X_ACK,015)
define(X_OKBLK,016)
define(X_ERBLK,017)
define(xmtctl,	{c1 = $1; c2 = $2;	xmtctl1()})
define(xmtcrc,	{crc16($1); xmt($1)})
define(incmod,	{++$1; if($1 >= $2)$1 = 0})
array crc[2];
array rtype[3];
array xtype[3];
function hasp()
#	trace(0);
	state = INIT;
	xbcb = 0xa0;
	xfcs0 = 0x8f;
	xfcs1 = 0xcf;
	xmtctl(SOH,ENQ);
#	trace(X_ENQ);
	do{
		rcvs = rec();
#		trace(rcvs);
		switch(state){
		case INIT:
			switch(rcvs){
			case R_ACK:
				xmtrsp();
			default:
				if(ct0<5){
					++ct0;
					xmtctl(SOH,ENQ);
#					trace(X_ENQ);
					state = INIT;
				}else{
					exitval = INITFLT;
					abort();
				}
			}
		case SENTACK:
			switch(rcvs){
			case R_ACK:
				ct1 = 0;
				xmtrsp();
			case R_OKBLK:
				ct1 = 0;
				rseqck();
			case R_ERBLK:
				ct1 = 0;
				xmtctl(NAK,NULL);
#				trace(X_NAK);
				state = SENTACK;
			case R_NAK:
				ct1 = 0;
				xmtctl(DLE,X70);
#				trace(X_ACK);
				state = SENTACK;
			default:
				++ct1;
				if(ct1 >= 6){
					exitval = ERRORS;
					abort();
				}else{
					xmtctl(NAK,NULL);
#					trace(X_NAK);
					state = SENTACK;
				}
			}
		case SENTBLK:
			switch(rcvs){
			case R_ACK:
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
#				trace(X_NAK);
				state = SENTBLK;
			case R_NAK:
				ct1 = 0;
				getxbuf(xtype);
				xmtblk();
				state = SENTBLK;
			default:
				++ct1;
				if(ct1 >= 6){
					exitval = ERRORS;
					abort();
				}else{
					xmtctl(NAK,NULL);
#					trace(X_NAK);
					state = SENTBLK;
				}
			}
		}
	}
end
function xmtblk()
	crcloc(crc);
	xsom(SYNC);
	xmt(DLE);
	xmt(STX);
	xmtcrc(xbcb);
	xmtcrc(xfcs0);
	xmtcrc(xfcs1);
	while(get(byte) == 0){
		if(byte == DLE)
			xmt(DLE);
		xmtcrc(byte);
	}
	xmt(DLE);
	xmtcrc(ETB);
	xmt(crc[0]);
	xmt(crc[1]);
	xeom(PAD);
#	trace(X_OKBLK);
end
function xmtdat()
	if(byte == DLE)
		xmt(DLE);
	xmtcrc(byte);
end
function xmtctl1()
	xsom(SYNC);
	xmt(c1);
	if(c2 != NULL)
		xmt(c2);
	xeom(PAD);
end
function rec()
	crcloc(crc);
	if(timeout(30))
		return(TIMEOUT);
	rsom(SYNC);
	rcv(byte);
	switch(byte){
	case NAK:
		rcv(byte);
		byte &= 017;
		if(byte == 017)
			return(R_NAK);
		else
			return(R_ERROR);
	case DLE:
		rcv(byte);
		switch(byte){
		case X70:
			rcv(byte);
			byte &= 017;
			if(byte == 017)
				return(R_ACK);
			else
				return(R_ERROR);
		case STX:
			while(getrbuf(rtype))
				;
			rbcb = rcvdat();
			put(0);
			rfcs0 = rcvdat();
			put(rfcs0);
			rfcs1 = rcvdat();
			put(rfcs1);
			do {
				byte = rcvdat();
				if((byte&0x05) == 0x05)
					punch = 1;
				else
					punch = 0;
				put(byte);
				if(byte == 0)
					break;
				byte = rcvdat();
				put(byte);
				do {
					byte = rcvdat()
					put(byte);
					if(byte == 0)
						break;
					switch(byte&0xc0) {
					case 0x40:
						next;
					case 0x80:
						if(byte&0x20) {
							byte = rcvdat();
							if(!punch)
								byte = etoa(byte);
							put(byte);
						}
					case 0xc0:
						count = byte&0x3f;
						for(;count;--count) {
							byte = rcvdat();
							if(!punch)
								byte = etoa(byte);
							put(byte);
						}
					}
				}
			}
			byte = rcvdat();
			if(byte != ETB)
				return(R_ERROR);
			rcvcrc();
			rcvcrc();
			rcv(byte);
			byte &= 017;
			if(byte != 017)
				return(R_ERROR);
			if(crc[0] != 0)
				return(R_ERBLK);
			if(crc[1] != 0)
				return(R_ERBLK);
			return(R_OKBLK);
		default:
			return(R_ERROR);
		}
	default:
		return(R_ERROR);
	}
end
function rcvcrc()
	rcv(byte);
	crc16(byte);
	return(byte);
end
function rcvdat()
	do {
		rcv(byte);
		if(byte != DLE) {
			crc16(byte);
			return(byte);
		} else {
			rcv(byte)
			if(byte == SYNC)
				next
			crc16(byte);
			return(byte);
		}
	}
end
function xmtserr()
	crcloc(crc);
	xsom(SYNC);
	xmt(DLE);
	xmt(STX);
	xmtcrc(actseq);
	xmtcrc(xfcs0);
	xmtcrc(xfcs1);
	xmtcrc(0xe0);
	xmtcrc(rseq);
	xmtcrc(0);
	xmtcrc(0);
	xmt(DLE);
	xmtcrc(ETB);
	xmt(crc[0])
	xmt(crc[1]);
	xeom(PAD);
#	trace(X_OKBLK);
	state = SENTACK;
end
function xmtrsp()
	if(wait){
		xmtctl(DLE,X70);
#		trace(X_ACK);
		state = SENTACK;
		return;
	}
	if(getxbuf(xtype)){
		xmtctl(DLE,X70);
#		trace(X_ACK);
		state = SENTACK;
	}else{
		xmtblk();
		state = SENTBLK;
	}
end
function rseqck()
	if(rfcs0&WAITBIT){
		wait = 1;
#		trace(R_WAIT);
	}else{
		wait = 0;
	}
	switch(rbcb&0xf0){
		case CKSEQ:
			actseq = rbcb&017;
			if(actseq == rseq){
				incmod(rseq,16);
				rtnrbuf(rtype);
				xmtrsp();
			}else{
#				trace(R_SEQER);
				xmtserr();
			}
		case NOCK:
			rtnrbuf(rtype);
			xmtctl(DLE,X70);
#			trace(X_ACK);
			state = SENTACK;
			return;
		case RSSEQ:
			rseq = rbcb&0x0f;
			rtnrbuf(rtype);
	}
end

function abort()
	while(getrbuf(rtype))
		;
	put(exitval);
	rtnrbuf(rtype);
	while(1)
	  ;
end
