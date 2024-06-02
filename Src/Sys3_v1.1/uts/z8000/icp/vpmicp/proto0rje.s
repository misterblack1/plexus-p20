#define	JMP	0200
#define	JMPEQZ	0220
#define	JMPNEZ	0240
#define	CALL	0260
#define	AND	1
#define	OR	2
#define	XOR	3
#define	BIC	4
#define	ADD	5
#define	SUB	6
#define	TSTEQL	7
#define	TSTNEQ	8
#define	TSTGTR	9
#define	TSTLSS	10
#define	TSTGEQ	11
#define	TSTLEQ	12
#define	MOVI	13
#define	ANDI	14
#define	ORI	15
#define	XORI	16
#define	BICI	17
#define	ADDI	18
#define	SUBI	19
#define	TSTEQLI	20
#define	TSTNEQI	21
#define	TSTGTRI	22
#define	TSTLSSI	23
#define	TSTGEQI	24
#define	TSTLEQI	25
#define	INC	26
#define	DEC	27
#define	CRC16	28
#define	XMT	29
#define	XMTI	30
#define	RCV	31
#define	CLR	32
#define	MOVM	33
#define	ORM	34
#define	XORM	35
#define	BICM	36
#define	ADDM	37
#define	SUBM	38
#define	GETBYTE	39
#define	PUTBYTE	40
#define	PUTBYTEI	41
#define	TRACE1	42
#define	RTNRBUF	43
#define	RTNXBUF	44
#define	TESTOP	45
#define	GETXBUF	46
#define	GETRBUF	47
#define	RETURN	48
#define	RETURNI	49
#define	HALT	50
#define	HALTI	51
#define	RSRBUF	52
#define	TIMEOUTI	53
#define	TRACE1I	54
#define	XEOMI	55
#define	XSOMI	56
#define	RSOMI	57
#define	CRC16I	58
#define	RSXBUF	59
#define	CRCLOC	60
#define	MOV	61
#define	DSRWAIT	62
#define	ANDM	63
#define	LSH	64
#define	LSHI	65
#define	LSHM	66
#define	RSH	67
#define	RSHI	68
#define	RSHM	69
#define	ETOA	70
#define	ATOE	71
#define	ETOAI	72
#define	ATOEI	73
#define	RCVFRM	74
#define	RSRFRM	75
#define	RTNRFRM	76
#define	XMTBUSY	77
#define	SETCTL	78
#define	XMTCTL	79
#define	GETXFRM	80
#define	XMTFRM	81
#define	RTNXFRM	82
#define	RSXMTQ	83
#define	ABTXFRM	84
#define	NORBUF	85
#define	GETCMD	86
#define	RTNRPT	87
#define	TIMERI	88
#define	TRACE2	89
#define	TRACE2I	90
#define	SNAP	91
#define	SSTACK	220
.byte TRACE1I,0
.byte GETXBUF,225
.byte TSTNEQI,0
.byte JMPEQZ,10
.byte JMP,2
.byte MOVI,020
.byte MOVM,228
.byte MOVI,0xa0
.byte MOVM,229
.byte MOVI,0x8f
.byte MOVM,230
.byte MOVI,0xcf
.byte MOVM,231
.byte MOVI,0x01
.byte MOVM,232
.byte MOVI,0x2d
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,013
.byte CALL|1,186
.byte MOVM,234
.byte TRACE1,234
.byte MOV,228
.byte MOVM,235
.byte TSTEQLI,020
.byte JMPEQZ,98
.byte MOV,234
.byte MOVM,236
.byte TSTEQLI,1
.byte JMPEQZ,64
.byte CALL|3,70
.byte JMP,38
.byte MOV,237
.byte TSTLSSI,5
.byte JMPEQZ,90
.byte INC,237
.byte MOVI,0x01
.byte MOVM,232
.byte MOVI,0x2d
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,013
.byte MOVI,020
.byte MOVM,228
.byte JMP,38
.byte MOVI,1
.byte MOVM,238
.byte CALL|3,220
.byte JMP,38
.byte MOV,235
.byte TSTEQLI,021
.byte JMPEQZ,216
.byte MOV,234
.byte MOVM,239
.byte TSTEQLI,1
.byte JMPEQZ,120
.byte CLR,240
.byte CLR,241
.byte CALL|3,70
.byte JMP,38
.byte MOV,239
.byte TSTEQLI,7
.byte JMPEQZ,132
.byte CLR,240
.byte CALL|3,122
.byte JMP,38
.byte MOV,239
.byte TSTEQLI,5
.byte JMPEQZ,158
.byte CLR,240
.byte MOVI,0x3d
.byte MOVM,232
.byte MOVI,0x00
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,014
.byte MOVI,021
.byte MOVM,228
.byte JMP,38
.byte MOV,239
.byte TSTEQLI,6
.byte JMPEQZ,184
.byte CLR,240
.byte MOVI,0x10
.byte MOVM,232
.byte MOVI,0x70
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,015
.byte MOVI,021
.byte MOVM,228
.byte JMP,38
.byte INC,240
.byte TSTGEQI,6
.byte JMPEQZ,198
.byte MOVI,2
.byte MOVM,238
.byte CALL|3,220
.byte JMP,38
.byte MOVI,0x3d
.byte MOVM,232
.byte MOVI,0x00
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,014
.byte MOVI,021
.byte MOVM,228
.byte JMP,38
.byte MOV,235
.byte TSTEQLI,022
.byte JMPEQZ,38
.byte MOV,234
.byte MOVM,242
.byte TSTEQLI,1
.byte JMPEQZ,252
.byte CLR,240
.byte MOVI,0x80
.byte OR,243
.byte MOVM,229
.byte INC,243
.byte TSTGEQI,16
.byte JMPEQZ,246
.byte CLR,243
.byte RTNXBUF,225
.byte CALL|3,70
.byte JMP,38
.byte MOV,242
.byte TSTEQLI,7
.byte JMPEQZ|1,24
.byte CLR,240
.byte MOVI,0x80
.byte OR,243
.byte MOVM,229
.byte INC,243
.byte TSTGEQI,16
.byte JMPEQZ|1,18
.byte CLR,243
.byte RTNXBUF,225
.byte CALL|3,122
.byte JMP,38
.byte MOV,242
.byte TSTEQLI,5
.byte JMPEQZ|1,50
.byte CLR,240
.byte MOVI,0x3d
.byte MOVM,232
.byte MOVI,0x00
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,014
.byte MOVI,022
.byte MOVM,228
.byte JMP,38
.byte MOV,242
.byte TSTEQLI,6
.byte JMPEQZ|1,68
.byte CLR,240
.byte GETXBUF,225
.byte CALL|1,100
.byte MOVI,022
.byte MOVM,228
.byte JMP,38
.byte INC,240
.byte TSTGEQI,6
.byte JMPEQZ|1,82
.byte MOVI,2
.byte MOVM,238
.byte CALL|3,220
.byte JMP,38
.byte MOVI,0x3d
.byte MOVM,232
.byte MOVI,0x00
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,014
.byte MOVI,022
.byte MOVM,228
.byte JMP,38
.byte CRCLOC,220
.byte XSOMI,0x32
.byte XMTI,0x10
.byte XMTI,0x02
.byte CRC16,229
.byte XMT,229
.byte CRC16,230
.byte XMT,230
.byte CRC16,231
.byte XMT,231
.byte GETBYTE,244
.byte TSTEQLI,0
.byte JMPEQZ|1,140
.byte MOV,244
.byte TSTEQLI,0x10
.byte JMPEQZ|1,134
.byte XMTI,0x10
.byte CRC16,244
.byte XMT,244
.byte JMP|1,120
.byte XMTI,0x10
.byte CRC16I,0x26
.byte XMTI,0x26
.byte XMT,220+0
.byte XMT,220+1
.byte XEOMI,0xff
.byte TRACE1I,016
.byte RETURNI,0
.byte MOV,244
.byte TSTEQLI,0x10
.byte JMPEQZ|1,164
.byte XMTI,0x10
.byte CRC16,244
.byte XMT,244
.byte RETURNI,0
.byte XSOMI,0x32
.byte XMT,232
.byte MOV,233
.byte TSTNEQI,0x00
.byte JMPEQZ|1,182
.byte XMT,233
.byte XEOMI,0xff
.byte RETURNI,0
.byte CRCLOC,220
.byte TIMEOUTI,30
.byte JMPEQZ|1,194
.byte RETURNI,012
.byte RSOMI,0x32
.byte RCV,244
.byte MOV,244
.byte MOVM,245
.byte TSTEQLI,0x3d
.byte JMPEQZ|1,222
.byte RCV,244
.byte MOVI,017
.byte ANDM,244
.byte MOV,244
.byte TSTEQLI,017
.byte JMPEQZ|1,220
.byte RETURNI,6
.byte RETURNI,011
.byte MOV,245
.byte TSTEQLI,0x10
.byte JMPEQZ|2,234
.byte RCV,244
.byte MOV,244
.byte MOVM,246
.byte TSTEQLI,0x70
.byte JMPEQZ|1,254
.byte RETURNI,1			/* was RCV,244 */
.byte MOVI,017
.byte ANDM,244
.byte MOV,244
.byte TSTEQLI,017
.byte JMPEQZ|1,252
.byte RETURNI,1
.byte RETURNI,011
.byte MOV,246
.byte TSTEQLI,0x02
.byte JMPEQZ|2,232
.byte GETRBUF,222
.byte JMPEQZ|2,10
.byte JMP|2,4
.byte CALL|2,244
.byte MOVM,247
.byte PUTBYTEI,0
.byte CALL|2,244
.byte MOVM,248
.byte PUTBYTE,248
.byte CALL|2,244
.byte MOVM,249
.byte PUTBYTE,249
.byte CALL|2,244
.byte MOVM,244
.byte ANDI,0x05
.byte TSTEQLI,0x05
.byte JMPEQZ|2,44
.byte MOVI,1
.byte MOVM,250
.byte JMP|2,46
.byte CLR,250
.byte PUTBYTE,244
.byte MOV,244
.byte TSTEQLI,0
.byte JMPEQZ|2,56
.byte JMP|2,154
.byte CALL|2,244
.byte MOVM,244
.byte PUTBYTE,244
.byte CALL|2,244
.byte MOVM,244
.byte PUTBYTE,244
.byte MOV,244
.byte TSTEQLI,0
.byte JMPEQZ|2,76
.byte JMP|2,28
.byte MOV,244
.byte ANDI,0xc0
.byte MOVM,251
.byte TSTEQLI,0x40
.byte JMPEQZ|2,88
.byte JMP|2,62
.byte MOV,251
.byte TSTEQLI,0x80
.byte JMPEQZ|2,118
.byte MOV,244
.byte ANDI,0x20
.byte JMPEQZ|2,62
.byte CALL|2,244
.byte MOVM,244
.byte MOV,250
.byte TSTEQLI,0
.byte JMPEQZ|2,114
.byte ETOA,244
.byte MOVM,244
.byte PUTBYTE,244
.byte JMP|2,62
.byte MOV,251
.byte TSTEQLI,0xc0
.byte JMPEQZ|2,62
.byte MOV,244
.byte ANDI,0x3f
.byte MOVM,252
.byte MOV,252
.byte JMPEQZ|2,62
.byte CALL|2,244
.byte MOVM,244
.byte MOV,250
.byte TSTEQLI,0
.byte JMPEQZ|2,148
.byte ETOA,244
.byte MOVM,244
.byte PUTBYTE,244
.byte DEC,252
.byte JMP|2,130
.byte CALL|2,244
.byte MOVM,244
.byte TSTNEQI,0x26
.byte JMPEQZ|2,196
.byte MOV,250
.byte TSTEQLI,0
.byte JMPEQZ|2,188
.byte TRACE1,244
.byte CALL|2,244
.byte MOVM,244
.byte TRACE1,244
.byte CALL|2,244
.byte MOVM,244
.byte TRACE1,244
.byte CALL|2,244
.byte MOVM,244
.byte TRACE1,244
.byte MOV,244
.byte TSTNEQI,0x26
.byte JMPEQZ|2,196
.byte RETURNI,011
.byte CALL|2,238
.byte CALL|2,238
.byte RCV,244
.byte MOVI,017
.byte ANDM,244
.byte MOV,244
.byte TSTNEQI,017
.byte JMPEQZ|2,214
.byte RETURNI,011
.byte MOV,220+0
.byte TSTNEQI,0
.byte JMPEQZ|2,222
.byte RETURNI,5
.byte MOV,220+1
.byte TSTNEQI,0
.byte JMPEQZ|2,230
.byte RETURNI,5
.byte RETURNI,7
.byte RETURNI,011
.byte RETURNI,011
.byte RETURNI,0
.byte RCV,244
.byte CRC16,244
.byte RETURN,244
.byte RCV,244
.byte MOV,244
.byte TSTNEQI,0x10
.byte JMPEQZ|3,0
.byte CRC16,244
.byte RETURN,244
.byte RCV,244
.byte MOV,244
.byte TSTEQLI,0x32
.byte JMPEQZ|3,10
.byte JMP|2,244
.byte CRC16,244
.byte RETURN,244
.byte CRCLOC,220
.byte XSOMI,0x32
.byte XMTI,0x10
.byte XMTI,0x02
.byte CRC16,253
.byte XMT,253
.byte CRC16,230
.byte XMT,230
.byte CRC16,231
.byte XMT,231
.byte CRC16I,0xe0
.byte XMTI,0xe0
.byte CRC16,254
.byte XMT,254
.byte CRC16I,0
.byte XMTI,0
.byte CRC16I,0
.byte XMTI,0
.byte XMTI,0x10
.byte CRC16I,0x26
.byte XMTI,0x26
.byte XMT,220+0
.byte XMT,220+1
.byte XEOMI,0xff
.byte TRACE1I,016
.byte MOVI,021
.byte MOVM,228
.byte RETURNI,0
.byte MOV,241
.byte JMPEQZ|3,92
.byte MOVI,0x10
.byte MOVM,232
.byte MOVI,0x70
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,015
.byte MOVI,021
.byte MOVM,228
.byte RETURNI,0
.byte GETXBUF,225
.byte JMPEQZ|3,114
.byte MOVI,0x10
.byte MOVM,232
.byte MOVI,0x70
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,015
.byte MOVI,021
.byte MOVM,228
.byte JMP|3,120
.byte CALL|1,100
.byte MOVI,022
.byte MOVM,228
.byte RETURNI,0
.byte MOV,248
.byte ANDI,0x40
.byte JMPEQZ|3,136
.byte MOVI,1
.byte MOVM,241
.byte TRACE1I,2
.byte JMP|3,138
.byte CLR,241
.byte MOV,247
.byte ANDI,0xf0
.byte MOVM,255
.byte TSTEQLI,0x80
.byte JMPEQZ|3,178
.byte MOV,247
.byte ANDI,017
.byte MOVM,253
.byte TSTEQL,254
.byte JMPEQZ|3,172
.byte INC,254
.byte TSTGEQI,16
.byte JMPEQZ|3,166
.byte CLR,254
.byte RTNRBUF,222
.byte CALL|3,70
.byte JMP|3,218
.byte TRACE1I,010
.byte CALL|3,14
.byte JMP|3,218
.byte MOV,255
.byte TSTEQLI,0x90
.byte JMPEQZ|3,204
.byte RTNRBUF,222
.byte MOVI,0x10
.byte MOVM,232
.byte MOVI,0x70
.byte MOVM,233
.byte CALL|1,170
.byte TRACE1I,015
.byte MOVI,021
.byte MOVM,228
.byte RETURNI,0
.byte MOV,255
.byte TSTEQLI,0xa0
.byte JMPEQZ|3,218
.byte MOV,247
.byte ANDI,0x0f
.byte MOVM,254
.byte RTNRBUF,222
.byte RETURNI,0
.byte GETRBUF,222
.byte JMPEQZ|3,226
.byte JMP|3,220
.byte PUTBYTE,238
.byte RTNRBUF,222
.byte MOVI,1
.byte JMPEQZ|3,236
.byte JMP|3,230
.byte RETURNI,0
