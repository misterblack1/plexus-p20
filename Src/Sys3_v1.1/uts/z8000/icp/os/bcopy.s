	.text
#ifdef COUNT
	.globl	_profile
#endif
	.globl	_bcopy,_wcopy
_bcopy:
~~bcopy:
#ifdef COUNT
	inc	_profile+0x42	//profile increment
#endif
	ld	r5,6(r15)	//r5 contains count
	ld	r6,4(r15)	//r6 contains destination
	ld	r7,2(r15)	//r7 contains source
	ldirb	*r6,*r7,r5
	ret			//r7 contains address of next block to move

_wcopy:
~~wcopy:
#ifdef COUNT
	inc	_profile+0x94	//profile increment
#endif
	ld	r5,6(r15)	//r5 contains count
	ld	r6,4(r15)	//r6 contains destination
	ld	r7,2(r15)	//r7 contains source

	srl	r5		//convert byte count to word count
				//carry set if odd byte count
	jr	z,L1		//zero flag set if 0, (1 char move)

	ldir	*r6,*r7,r5
	ret	nc		//r7 contains address of nxt block to move
L1:
	ldb	rl5,*r7		//take care of single char move
	ldb	*r6,rl5		//faster than ldib
	inc	r7,$1
	ret			//r7 contains address of nxt block to move

	.data

