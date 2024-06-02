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
	br	movd		/* MOV */
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
	br	trace		/* TRACE */
	br	rtnrbuf		/* RTNRBUF */
	br	rtnxbuf		/* RTNXBUF */
	br	testop		/* TESTOP */
	br	getxbuf		/* GETXBUF */
	br	getrbuf		/* GETRBUF */
	br	popj		/* RETURN */
	br	popji		/* RETURNI */
	br	exit		/* EXIT */
	br	exiti		/* EXITI */
	br	rsrbuf		/* RSRBUF */
	br	timeout		/* TIMEOUT */
	br	tracei		/* TRACEI */
	br	xeomi		/* XEOMI */
	br	xsomi		/* XSOMI */
	br	rsomi		/* RSOMI */
	br	crc16i		/* CRC16I */
	br	rsxbuf		/* RSXBUF */
	br	crcloc		/* CRCLOC */
	br	andm		/* ANDM */
	br	dsrwait		/* DSRWAIT */
lastop:
