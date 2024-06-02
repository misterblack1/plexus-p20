//getc - get a character from a clist
	.text
	.globl	_getc
_getc:
~~getc:
#ifdef COUNT
	inc	_profile+0x36,$1
#endif
	ldctl	r0,fcw		//save current processor status
	di	vi		//spl4()
	ld	r6,2(r15)	//r6 = &ptr->c_cc = &clist
	dec	*r6,$1		//decrement character count in clist
	jr	lt,L1

	ld	r5,2(r6)	//r5 = ptr->c_cf = &cblock.c_next
	incb	2(r5),$1	//++ptr->c_cf->c_first

	ldb	rl4,2(r5)	//rl4 = ++ptr->c_cf->c_first
	extsb	r4

	ld	r7,r4
	add	r7,r5		//r7 = index + c_data - 3
	ldb	rl7,3(r7)	//skip link & 2 index bytes (+1 already + 3)
	clrb	rh7

	cpb	rl4,3(r5)	//compare c_first against c_last in cblock
	jr	nz,L3		//have more data in cblock

	ld	r4,*r5		//r5 = &ptr->c_cf->c_next
	ld	2(r6),r4	//r6+2 = &ptr->c_cf

	test	r4		//if ptr->c_cf == 0 then  ptr->c_cl <= 0
	jr	nz,L2

	clr	4(r6)		//ptr->c_cl <= 0
L2:
	ld	r4,_cfreeli	//get link to fisrt free cblock
	ld	*r5,r4		//put it into link of new free cblock
	ld	_cfreeli,r5	//put address of new free cblock into cfreelist
L3:
	ldctl	fcw,r0		//restore processor status
	ret
L1:
	clr	*r6
	ldctl	fcw,r0		//restore processor status
	ld	r7,$-1
	ret

	.data

//putc - put a character at the end of a clist
	.text
	.globl	_putc
_putc:
~~putc:
#ifdef COUNT
	inc	_profile+0x38,$1
#endif
	ldctl	r0,fcw		//save current processor status
	di	vi		//spl4()
	ld	r6,4(r15)	//r6 = &ptr->c_cc = &clist
	ld	r5,4(r6)	//r5 = ptr->c_cl
	test	r5		//if(ptr->c_cl != 0) go to L1
	jr	z,L4

	ldb	rl4,3(r5)	//r4(low) = ptr->c_cl->c_last
	cpb	rl4,_cfreelist+3
	jr	lt,L5		//if(ptr->c_cl->c_last >= cfreelist.c_size)

	ld	r3,_cfreelist	//r3 = cfreelist.c_next
	test	r3		//if(cfreelist empty) goto L3
	jr	z,L6

	ld	*r5,r3		//ptr->cl->c_next = cfreelist.c_next

	ld	r2,*r3
	ld	_cfreelist,r2	//cfreelist.c_next = cfreelist.c_next->c_next
	ld	4(r6),r3	//ptr->c_cl = r3
	clr	*r3		//ptr->c_cl->c_next = 0
	ld	2(r3),$1	//ptr->c_cl->c_first=0,ptr->c_cl->c_last=1
	ld	r2,2(r15)	//r2 = character
	ldb	4(r3),rl2	//ptr->c_cl->c_data[0] = c
	inc	*r6		//ptr->c_cc++
	ldk	r7,$0
	ldctl	fcw,r0		//splx(r0)
	ret

L4:
	ld	r3,_cfreelist	//r3 = cfreelist.c_next
	test	r3		//if(cfreelist empty) goto L3
	jr	z,L6

	ld	2(r6),r3	//ptr->c_cf = r3

	ld	r2,*r3
	ld	_cfreelist,r2	//cfreelist.c_next = cfreelist.c_next->c_next
	ld	4(r6),r3	//ptr->c_cl = r3
	clr	*r3		//ptr->c_cl->c_next = 0
	ld	2(r3),$1	//ptr->c_cl->c_first=0,ptr->c_cl->c_last=1
	ld	r2,2(r15)	//r2 = character
	ldb	4(r3),rl2	//ptr->c_cl->c_data[0] = c
	inc	*r6		//ptr->c_cc++
	ldk	r7,$0
	ldctl	fcw,r0		//splx(r0)
	ret
L5:
	inc	*r6		//ptr->c_cc++
	incb	3(r5)		//ptr->c_cl->c_last++
	extsb	r4
	add	r5,r4
	ld	r2,2(r15)	//r2 = character
	ldb	4(r5),rl2	//ptr->c_cl->c_data[ptr->c_cl->c_last] = char
	ldk	r7,$0
	ldctl	fcw,r0		//splx(r0)
	ret
L6:
	ld	r7,$-1
	ldctl	fcw,r0		//splx(r0)
	ret

	.data

//getcb - get a cblock from the start of a clist
	.text
	.globl	_getcb
_getcb:
~~getcb:
#ifdef COUNT
	inc	_profile+0x3a,$1
#endif
	ld	r6,2(r15)	//r6 = &ptr->c_cc = &clist
	ldctl	r0,fcw		//save processor status
	di	vi		//spl4()
	ld	r7,2(r6)	//r7 = ptr->c_cf
	test	r7
	jr	z,L7		//if its 0, then just return.

	ldb	rl5,3(r7)	//r5 = ptr->c_cf->c_last
	subb	rl5,2(r7)	//r5 = c_last - c_first
	extsb	r5

	sub	r5,*r6		//char count = char count - # char in cblock
	neg	r5
	ld	*r6,r5

	ld	r5,*r7		//r5 = ptr->c_cf->c_next
	ld	2(r6),r5	//ptr->c_cf = ptr->c_cf->c_next (reg 5)

	test	r5
	jr	nz,L7
	clr	4(r6)		//if new ptr->c_cf == 0 then make ptr->c_cl = 0
L7:
	ldctl	fcw,r0		//restore processor status
	ret

	.data

//putcb - put a cblock at the end of a clist
	.text
	.globl	_putcb
_putcb:
~~putcb:
#ifdef COUNT
	inc	_profile+0x3c,$1
#endif
	ld	r6,4(r15)	//r6 = clptr
	ld	r4,2(r15)	//r4 = cbptr
	ldctl	r0,fcw		//save processor status
	di	vi		//r0=spl4()
	ld	r5,4(r6)	//r5 = ptr->c_cl
	test	r5
	jr	z,L8		//if(clptr->c_cl == 0)

	ld	*r5,r4		//    clptr->c_cl->c_next = cbptr
	jr	L9		//else
L8:
	ld	2(r6),r4	//    clptr->c_cf = cbptr
L9:
	ld	4(r6),r4	//clptr->c_cl = cbptr
	clr	*r4		//cbptr->c_next = 0

	ldb	rl3,3(r4)
	subb	rl3,2(r4)	//cbptr->c_last - cbptr->c_first
	extsb	r3
	add	r3,*r6		//adjust char count in queue
	ld	*r6,r3

	ldk	r7,$0
	ldctl	fcw,r0		//splx(r0)

	ret

	.data

//getcf - get a cblock from the free clist pool
	.text
	.globl	_getcf
_getcf:
~~getcf:
#ifdef COUNT
	inc	_profile+0x3e,$1
#endif
	ldctl	r0,fcw		//save processor status
	di	vi		//spl4()
	ld	r7,_cfreelist	//r7 = _cfreelist
	test	r7
	jr	z,L10		//if its 0, then just return.

	ld	r6,*r7
	ld	_cfreelist,r6	//cfreelist.c_next = cfreelist.c_next->c_next

	clr	*r7		//clear c_next in cblock
	ld	r6,_cfreelist+2
	ld	2(r7),r6	//c_first = 0; c_last = CLSIZE
L10:
	ldctl	fcw,r0		//restore processor status
	ret

	.data

//putcf - return a cblock to the free cblock pool
	.text
	.globl	_putcf
_putcf:
~~putcf:
#ifdef COUNT
	inc	_profile+0x40,$1
#endif
	ld	r7,2(r15)	//r7 = cbptr
	ldctl	r0,fcw		//save processor status
	di	vi		//r0=spl4()

	ld	r6,_cfreelist
	ld	*r7,r6		//cbptr.c_next = cfreelist.c_next
	ld	_cfreelist,r7	//cfreelist.c_next = cbptr

	ldctl	fcw,r0		//splx(r0)
	ret

	.data
