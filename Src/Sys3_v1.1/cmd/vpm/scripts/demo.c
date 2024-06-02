/*
 * The function defined below transmits a frame in transparent bisync.
 * A transmit buffer must be obtained with getxbuf before the function
 * is invoked.
 *
 * Define symbolic constants:
 */
#define	DLE	0x10
#define	ETB	0x26
#define	PAD	0xff
#define	STX	0x02
#define	SYNC	0x32
/*
 * Define a macro with an argument:
 */
#define	xmtcrc(X)	{crc16(X); xmt(X);}
/*
 * Declare an array:
 */
array crc[2];
/*
 * Define the function:
 */
function xmtblk()
	crcloc(crc);
	xsom(SYNC);
	xmt(DLE);
	xmt(STX);
	while(get(byte)==0){
		if(byte == DLE)
			xmt(DLE);
		xmtcrc(byte);
	}
	xmt(DLE);
	xmtcrc(ETB);
	xmt(crc[0]);
	xmt(crc[1]);
	xeom(PAD);
end
/*
 * The following example illustrates the use of macros to simulate a
 * function call with arguments.
 *
 * The macro definition:
 */
#define	xmtctl(X,Y)	{c=X;d=Y;xmtctl1()}
/*
 * The function definition:
 */
function xmtctl1()
	xsom(SYNC);
	xmt(c);
	if(d!=0)
		xmt(d);
	xeom(PAD);
end
/*
 * Sample invocation:
 */
function main()
	xmtctl(DLE,NAK);
end
