jumpsw:
	br	jump		/* 0 - JMP */
	br	jumpeqz		/* 1 - JMPEQZ */
	br	jumpnez		/* 2 - JMPNEZ */
	br	pushj		/* 3 - CALL */
	br	illegal		/* 4 - unused */
	br	illegal		/* 5 - unused */
	br	illegal		/* 6 - unused */
	br	illegal		/* 7 - unused */
opsw:
	br	illegal		/* unused op code */
	br	andd		/* AND */
	br	ord		/* OR */
	br	xord		/* XOR */
	br	bicd		/* BIC */
	br	addd		/* ADD */
	br	subd		/* SUB */
	br	tsteqld		/* TSTEQL */
	br	tstneqd		/* TSTNEQ */
	br	tstgtrd		/* TSTGTR */
	br	tstlssd		/* TSTLSS */
	br	tstgeqd		/* TSTGEQ */
	br	tstleqd		/* TSTLEQ */
	br	movi		/* MOVI */
	br	andi		/* ANDI */
	br	ori		/* ORI */
	br	xori		/* XORI */
	br	bici		/* BICI */
	br	addi		/* ADDI */
	br	subi		/* SUBI */
	br	tsteqli		/* TSTEQLI */
	br	tstneqi		/* TSTNEQI */
	br	tstgtri		/* TSTGTRI */
	br	tstlssi		/* TSTLSSI */
	br	tstgeqi		/* TSTGEQI */
	br	tstleqi		/* TSTLEQI */
	br	incm		/* INC */
	br	decm		/* DEC */
	br	crc16		/* CRC16 */
	br	xmt		/* XMT */
	br	xmti		/* XMTI */
	br	rcv		/* RCV */
	br	clrm		/* CLR */
	br	movm		/* MOVM */
	br	orm		/* ORM */
	br	xorm		/* XORM */
	br	bicm		/* BICM */
	br	addm		/* ADDM */
	br	subm		/* SUBM */
	br	getbyte		/* GETBYTE */
	br	putbyte		/* PUTBYTE */
	br	putbytei	/* PUTBYTEI */
	br	trace1		/* TRACE1 */
	br	rtnrbuf		/* RTNRBUF */
	br	rtnxbuf		/* RTNXBUF */
	br	testop		/* TESTOP */
	br	getxbuf		/* GETXBUF */
	br	getrbuf		/* GETRBUF */
	br	popj		/* RETURN */
	br	popji		/* RETURNI */
	br	exit		/* EXIT */
	br	exiti		/* EXITI */
	br	illegal		/* RSRBUF */
	br	timeout		/* TIMEOUT */
	br	trace1i		/* TRACE1I */
	br	xeomi		/* XEOMI */
	br	xsomi		/* XSOMI */
	br	rsomi		/* RSOMI */
	br	crc16i		/* CRC16I */
	br	illegal		/* RSXBUF */
	br	crcloc		/* CRCLOC */
	br	movd		/* MOV */
	br	dsrwait		/* DSRWAIT */
	br	andm		/* ANDM */
	br	lshd		/* LSH */
	br	lshi		/* LSHI */
	br	lshm		/* LSHM */
	br	rshd		/* RSH */
	br	rshi		/* RSHI */
	br	rshm		/* RSHM */
	br	etoad		/* ETOA */
	br	atoed		/* ATOE */
	br	etoai		/* ETOAI */
	br	atoei		/* ATOEI */
	br	rcvfrm		/* RCVFRM */
	br	rsrfrm		/* RSRFRM */
	br	rtnrfrm		/* RTNRFRM */
	br	xmtbusy		/* XMTBUSY */
	br	setctli		/* SETCTL */
	br	xmtctl		/* XMTCTL */
	br	getxfrm		/* GETXFRM */
	br	xmtfrm		/* XMTFRM */
	br	rtnxfrm		/* RTNXFRM */
	br	rsxmtq		/* RSXMTQ */
	br	abtxfrm		/* ABTXFRM */
	br	norbuf		/* NORBUF */
	br	getcmd		/* GETCMD */
	br	rtnrpt		/* RTNRPT */
	br	timer		/* TIMER */
	br	trace2		/* TRACE2 */
	br	trace2i		/* TRACE2I */
	br	snap		/* SNAP */
lastop:
