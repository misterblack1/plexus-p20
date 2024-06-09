#include	"ctype.h"


char	_ctype[]={
/*	000	001	002	003	004	005	006	007	*/
	_EOF,	0,	0,	0,	0,	0,	0,	0,

/*	bs	ht	nl	vt	np	cr	so	si	*/
	0,	_S,	_EOR,	0,	0,	0,	0,	0,

	0,	0,	0,	0,	0,	0,	0,	0,

	0,	0,	0,	0,	0,	0,	0,	0,

/*	sp	!	"	#	$	%	&	'	*/
	_S,	0,	0,	0,	_DOLLAR,0,	0,	0,

/*	(	)	*	+	,	-	.	/	*/
	_BRA,	_KET,	_STAR,	_PLUS,	0,	_MINUS,	_DOT,	0,

/*	0	1	2	3	4	5	6	7	*/
	_D,	_D,	_D,	_D,	_D,	_D,	_D,	_D,

/*	8	9	:	;	<	=	>	?	*/
	_D,	_D,	_COLON,	0,	0,	_EQ,	0,	_QUERY,

/*	@	A	B	C	D	E	F	G	*/
	0,	_U,	_U,	_U,	_U,	_U,	_U,	_U,

/*	H	I	J	K	L	M	N	O	*/
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,

/*	P	Q	R	S	T	U	V	W	*/
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,

/*	X	Y	Z	[	\	]	^	_	*/
	_U,	_U,	_U,	0,	0,	0,	_HAT,	_U,

/*	`	a	b	c	d	e	f	g	*/
	0,	_L,	_L,	_L,	_L,	_L,	_L,	_L,

/*	h	i	j	k	l	m	n	o	*/
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,

/*	p	q	r	s	t	u	v	w	*/
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,

/*	x	y	z	{	|	}	~	del	*/
	_L,	_L,	_L,	_CBRA,	_BAR,	_CKET,	0,	0
};

