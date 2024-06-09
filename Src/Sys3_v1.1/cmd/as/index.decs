
/* index.decs */
 
/* This table contains then index of the corresponding           */
/*   operand combination in the operand table.  It is necessary  */
/*   to change this table whenever changes are made to the       */
/*   operand table, making sure that the entries are in the      */
/*   same order in both tables.                                  */ 
 
#define	NIL_		0
#define	R_		1 + NIL_
#define	RB_		1 + R_
#define	RR_		1 + RB_
#define	RQ_		1 + RR_
#define	IR_		5
#define	DA_		1 + IR_
#define	X_		1 + DA_
#define	IM_		1 + X_
#define	RA_		1 + IM_
#define	RANEG_		10
#define	CC_		1 + RANEG_
#define	F_		1 + CC_
#define	INT_		1 + F_

#define	R_R		1 + INT_
#define	R_RB		15
#define R_RR		1 + R_RB
#define R_RQ		1 + R_RR
#define	R_IR		1 + R_RQ
#define R_IOR		1 + R_IR
#define	R_DA		1 + R_IOR
#define	R_X		1 + R_DA
#define	R_IM		1 + R_X
#define	R_RA		23
#define	R_RABYTE	1 + R_RA
#define RABYTE_R	1 + R_RABYTE
#define	R_RANEG		1 + RABYTE_R
#define	R_BA		1 + R_RANEG
#define	R_BX		1 + R_BA
#define	R_CTLR		29
 
#define	SR_DA		1 + R_CTLR
#define SR_X		1 + SR_DA
#define SR_BA		32
#define	SR_BX		1 + SR_BA
#define	SR_RABYTE	1 + SR_BX

#define	RB_R		1 + SR_RABYTE
#define	RB_RB		1 + RB_R
#define	RB_IR		1 + RB_RB
#define RB_IOR		1 + RB_IR
#define	RB_DA		1 + RB_IOR
#define	RB_X		40
#define	RB_IM		1 + RB_X
#define	RB_RA		1 + RB_IM
#define	RB_RANEG	1 + RB_RA
#define	RB_BA		1 + RB_RANEG
#define	RB_BX		45
#define	RB_FLGR		1 + RB_BX
 
#define	RR_R		1 + RB_FLGR
#define	RR_RB		1 + RR_R
#define	RR_RR		1 + RR_RB
#define RR_RQ		1 + RR_RR
#define	RR_IR		51
#define	RR_DA		1 + RR_IR
#define	RR_X		1 + RR_DA
#define	RR_IM		1 + RR_X
#define	RR_RA		1 + RR_IM
#define	RR_BA		56
#define	RR_BX		1 + RR_BA
 
#define RQ_R		1 + RR_BX
#define	RQ_RR		1 + RQ_R
#define RQ_RQ		1 + RQ_RR
#define	RQ_IR		1 + RQ_RQ
#define	RQ_DA		1 + RQ_IR
#define	RQ_X		63
#define	RQ_IM		1 + RQ_X

#define	IR_R		1 + RQ_IM
#define	IR_RR		1 + IR_R
#define	IR_RB		1 + IR_RR
#define	IR_IR		68
#define	IR_DA		1 + IR_IR
#define	IR_X		70
#define	IR_IM		1 + IR_X
 
#define IOR_R		1 + IR_IM
#define	IOR_RB		1 + IOR_R

#define	DA_R		1 + IOR_RB
#define	DA_RB		75
#define	DA_RR		1 + DA_RB
#define	DA_IR		1 + DA_RR
#define	DA_IM		1 + DA_IR
 
#define	X_R		1 + DA_IM
#define	X_RB		80
#define	X_RR		1 + X_RB
#define	X_IR		1 + X_RR
#define	X_IM		1 + X_IR
 
#define	RA_R		1 + X_IM
#define	RA_RB		85
#define	RA_RR		1 + RA_RB
 
#define	BA_R		1 + RA_RR
#define	BA_RB		1 + BA_R
#define	BA_RR		1 + BA_RB
 
#define	BX_R		90
#define	BX_RB		1 + BX_R
#define	BX_RR		1 + BX_RB
 
#define	CC_R		1 + BX_RR
#define	CC_RB		1 + CC_R
#define	CC_IR		95
#define	CC_DA		1 + CC_IR
#define	CC_X		1 + CC_DA
#define	CC_RA		1 + CC_X
 
#define	F_F		1 + CC_RA

#define	INT_INT		100

#define	CTLR_R		1 + INT_INT

#define	FLGR_R		1 + CTLR_R
#define	FLGR_RB		1 + FLGR_R
 
#define	R_IR_IM		1 + FLGR_RB
#define	R_DA_IM		106
#define	R_X_IM		2 + R_DA_IM
#define	IR_R_IM		110
#define	IR_IR_R		2 + IR_R_IM
#define	IR_IOR_R	2 + IR_IR_R
#define IOR_IR_R	2 + IR_IOR_R
#define	DA_R_IM		118
#define	X_R_IM		2 + DA_R_IM
#define	F_F_F		2 + X_R_IM 
#define	R_IR_R_CC       2 + F_F_F
#define	RB_IR_R_CC      127
#define	IR_IR_R_CC      3 + RB_IR_R_CC
#define	F_F_F_F		3 + IR_IR_R_CC
#define FR_FR		3 + F_F_F_F

