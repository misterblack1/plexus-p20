#		mod 40/4 terminal driver
#	any questions or problems on this script should be directed
#	to A. Sabsevitz  PY x7311.  This script has not been
#	fully tested yet, but is working with one MCC supporting
#	two ASCII KDs.
# script waits until a receive buffer is available
# first xmit buffer is special.  it contains the station-id
# to use for the general poll.
# once this has been gotten and the buffer freed, the machine
# is started in the POLL state.
# the script alternates between the general poll for receive data
# and trying to xmit something.
# this mechanism will be tuned later.
# NOTE that we do odd parity LRC checking and not CRC here.
define(do,repeat)
define(continue,next)
define(XTRY,2)
define(STRY,3)
define(PTRY,3)
define(MKODD,0200)
define(POFF,0177)
define(FAIL,1)
define(ZERO,0)
define(FIRST,040)
define(NOTLAST,2)
define(LAST,4)
define(SETUP,0126)
define(NOTSETUP,124)
define(QUOTE,0xA2)
define(POLL,8)
define(POLLR,16)
define(XMIT,24)
define(XMITR,32)
define(XMIT2,40)
define(XMIT2R,48)
define(SEL,56)
define(TMOUT,0176)

define(ACK0,0xB0)
define(ACK1,0x31)
define(RVI,0xBC)
define(WACK,0x3B)
define(EOT,0x04)
define(NULL,0)
define(NONE,128)
define(SOH,0x01)
define(STX,0x02)
define(ETX,0x83)
define(ENQ,0x85)
define(DLE,0x10)
define(NAK,0x15)
define(SYN,0x16)
define(PAD,0377)
define(ETB,0x97)

define(getstat, g1 = $1; g2 = $2; getstat1() )
define(selpoll, sp1 = $1; sp2 = $2; spoll() )
define(sendit, o1 = $1; o2 = $2; ackit() )
define(fail, ftype = $1; fail1() )
define(lrcalc, lrc ^= byte)


array xtype[3];
array rtype[3];

function mod40()
	bno = 1;
	retry = 0;
	state = POLL;
	while(getrbuf(rtype) != 0);
#	code to process first, special write.
	do {
		if (getxbuf(xtype)==0) {
			get(byte);
			if (byte == SETUP)
				break;
			else {
				xtype[0] = NOTSETUP | FAIL;
				rtnxbuf(xtype);
			}
		}
	}
	get(spa);
	get(ssa);
	rtnxbuf(xtype);
#	main loop
	do {
		trace(state);
		switch(state) {
#	if a receive buffer is available (nobuf == 0), send a
#	general poll to the specified station and enter state
#	POLLResponse.  Otherwise, see if anything to XMIT.
		case POLL:
			oack = NONE;
			ack = ACK1;
			stry = 0;
			if (retry > PTRY) {
				sendit(NULL,EOT);
				retry = 0;
			}
			if (nobuf)
				if (! getrbuf(rtype))
					nobuf = 0;
			if (nobuf)
				state = XMIT;
			else {
				selpoll(spa,QUOTE);
				state = POLLR;
				rtype[0] = FIRST;
			}
#	receive poll response.  EOT indicates nothing to
#	receive, so see if you have something to transmit.
#	a STX or SOH is a terminal message on its way in, so
#	receive the block.  a ENQ requests you to send the last
#	ACK again; the terminal didn't receive it.
		case POLLR:
			byte = getbt();
			trace(byte);
			switch(byte) {
			case ENQ:
				if (oack != NONE) {
					if (++retry > PTRY)
						state = POLL;
					else {
						sendit(DLE,oack);
					}
				}
				else
					state = POLL;
			case EOT:
				retry = 0;
				ack = ZERO;
				oack = NONE;
				state = XMIT;
#	SOH message, position yourself at following STX -- three bytes hence.
			case SOH:
				lrc = 0;
				perror = 0;
				for(i=0;i<3;++i) {
					put(byte);
					rcv(byte);
					if (! testop(byte))
						++perror;
					byte &= POFF;
					lrcalc();
				}
				goto gethd;
#	STX byte, get station and device for rtype values
#	and then receive rest of block
			case STX:
				lrc = 0;
				perror = 0;
gethd:
				put(byte);
				if (rtype[0] & FIRST) {
					rcv(byte);
					if (! testop(byte))
						++perror;
					byte &= POFF;
					sta = byte;
					lrcalc();
					rcv(byte);
					if (! testop(byte))
						++perror;
					byte &= POFF;
					dev = byte;
					lrcalc();
					put(sta);
					put(dev);
				}
				rcvblk();
			default:
				state = POLL;
				++retry;
			}
#	see if anything to send.  if not, go and send general poll again.
#	if xmit buffer available, get the station and device from it
#	and select the terminal.
		case XMIT:
			if (getxbuf(xtype))
				state = POLL;
			else
				state = SEL;
		case SEL:
			selpoll(ssa,xtype[2]);
			state = XMITR;
#	receive the response to the select.
#	ACK0 means go ahead.
#	WACK is terminal busy
#	RVI is S & S from terminal, so get the status
		case XMITR:
			byte = getbt();
			trace(byte);
			switch(byte) {
			case WACK:
				fail(EOT);
			case ACK0:
				ack = ACK1;
				retry = 0;
				state = XMIT2;
			case RVI:
				getstat(ssa,xtype[2]);
			default:
				if (++retry > XTRY) {
					fail(EOT);
				}
				else
					state = SEL;
			}
#	xmit the block
		case XMIT2:
			xmit();
			state = XMIT2R;
#	receive response to xmit.
#	ACK[10] is good transmission.
#	NAK is invalid LRC or bad data, so try again
#	WACK is tty can not receive any more data after that block
#	EOT is tty has S & S to send
		case XMIT2R:
			byte = getbt();
			trace(byte);
			switch(byte) {
			case WACK:
				rtnxbuf(xtype);
				sendit(NULL,EOT);
				state = POLL;
				retry = 0;
			case NAK:
				if (++retry < XTRY) {
					state = XMIT2;
					getxbuf(xtype);
				}
				else {
					fail(EOT);
				}
			case EOT:
				getstat(ssa,xtype[2]);
			case ACK0:
				goto acktst;
			case ACK1:
acktst:
				if (byte != ack)
					goto bad;
				ack ^= 0201;
				rtnxbuf(xtype);
				sendit(NULL,EOT);
				state = POLL;
				retry = 0;
			default:
bad:
				if (++retry < XTRY) {
					sendit(NULL,ENQ);
				}
				else {
					fail(EOT);
				}
			}
		default:
			state = XMIT;
		}
	}
end


function xmit()
	lrc = 0;
	xsom(SYN);
	if (get(byte) == 0)
		xmt(byte);
	while(get(byte) == 0) {
		lrcalc();
		if (! testop(byte))
			byte ^= MKODD;
		xmt(byte);
	}
	if (! testop(lrc))
		lrc ^= MKODD;
	xmt(lrc);
	xeom(PAD);
end
function getstat1()
	if (++stry > STRY) {
		fail(NULL);
	}
	else {
		selpoll(g1,g2);
		state = POLLR;
	}
end
function rcvblk()
	if (timeout(200))
		return(TMOUT);
    do {
	rcv(byte);
	if (byte == PAD)
		continue;
	if (byte == SYN)
		continue;
	if (! testop(byte))
		++perror;
	sbyte = byte;
	byte &= POFF;
	lrcalc();
	put(byte);
	switch(sbyte) {
	case ETX:
		rtype[0] &= ~NOTLAST;
		rtype[0] |= LAST;
		goto eob;
	case ETB:
		rtype[0] |= NOTLAST;
eob:
		rcv(byte);
		if (! testop(lrc))
			lrc ^= MKODD;
		if ((lrc != byte) | perror) {
			if (++retry > XTRY) {
				state = POLL;
				retry = 0;
				sendit(NULL,EOT);
			}
			else {
				sendit(NULL,NAK);
			}
			nobuf = getrbuf(rtype);
		}
		else {
			rtype[1] = sta;
			rtype[2] = dev;
			rt0 = rtype[0];
			rtnrbuf(rtype);
			if (rt0 & NOTLAST) {
				while(getrbuf(rtype));
				nobuf = 0;
			} else {
				nobuf = getrbuf(rtype);
			}
			if (nobuf) {
				sendit(DLE,RVI);
			}
			else {
				retry = 0;
				oack = ack;
				sendit(DLE,ack);
				ack ^= 0201;
				trace(ack);
			}
		}
		return;
	}
    }
end
function spoll()
	if (! testop(sp1))
		sp1 ^= MKODD;
	if (! testop(sp2))
		sp2 ^= MKODD;
	xsom(SYN);
	xmt(EOT);
	xmt(PAD);
	xmt(SYN);
	xmt(SYN);
	xmt(sp1);
	xmt(sp1);
	xmt(sp2);
	xmt(sp2);
	xmt(ENQ);
	xeom(PAD);
end
function getbt()
	if (timeout(50))
		return(TMOUT);
	rsom(SYN);
	rcv(byte);
	if (byte == DLE)
		rcv(byte);
	return(byte);
end
function ackit()
	trace(0377);
	xsom(SYN);
	if(o1 != NULL) {
		xmt(o1);
		trace(o1);
	}
	xmt(o2);
	trace(o2);
	xeom(PAD);
end
function fail1()
	state = POLL;
	xtype[0] = (byte | state) | FAIL;
	rtnxbuf(xtype);
	retry = 0;
	if (ftype) {
		sendit(NULL,ftype);
	}
end
