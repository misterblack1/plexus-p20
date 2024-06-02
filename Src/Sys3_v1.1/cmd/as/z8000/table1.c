#
#include "common.h"
#include "format.h"
#include "index.h"
#include "types.h"
 

OPCODENTRY opcodetable[] = {

               /*opcode*/     /*operand*/    /*format*/
                              /*  entry*/    /* entry*/
               
          /* ADC */
          {     0XB500,        R_R,           F0R },

          /* ADCB */
          {     0XB400,        RB_RB,         F0R },

          /* ADD */
          {     0X8100,        R_R,           F0R },
          {     0X0100,        R_IR,          F0R },
          {     0X4100,        R_DA,          F0DA },
          {     0X4100,        R_X,           F0X },
          {     0X0100,        R_IM,          F0IMW },

          /* ADDB */
          {     0X8000,        RB_RB,         F0R },
          {     0X0000,        RB_IR,         F0R },
          {     0X4000,        RB_DA,         F0DA },
          {     0X4000,        RB_X,          F0X },
          {     0X0000,        RB_IM,         F0IMB },

          /* ADDL */
          {     0X9600,        RR_RR,         F0R },
          {     0X1600,        RR_IR,         F0R },
          {     0X5600,        RR_DA,         F0DA },
          {     0X5600,        RR_X,          F0X },
          {     0X1600,        RR_IM,         F0IML },

          /* AND */
          {     0X8700,        R_R,           F0R },
          {     0X0700,        R_IR,          F0R },
          {     0X4700,        R_DA,          F0DA },
          {     0X4700,        R_X,           F0X },
          {     0X0700,        R_IM,          F0IMW },

          /* ANDB */
          {     0X8600,        RB_RB,         F0R },
          {     0X0600,        RB_IR,         F0R },
          {     0X4600,        RB_DA,         F0DA },
          {     0X4600,        RB_X,          F0X },
          {     0X0600,        RB_IM,         F0IMB },

          /* BIT */
          {     0XA700,        R_IM,          F1R },
          {     0X2700,        IR_IM,         F1R },
          {     0X6700,        DA_IM,         F1DA },
          {     0X6700,        X_IM,          F1X },
          {     0X2700,        R_R,           F12 },

          /* BITB */
          {     0XA600,        RB_IM,         F1R },
          {     0X2600,        IR_IM,         F1R },
          {     0X6600,        DA_IM,         F1DA },
          {     0X6600,        X_IM,          F1X },
          {     0X2600,        RB_R,          F12 },

          /* CALL */
          {     0X1F00,        IR_,           F2R },
          {     0X5F00,        DA_,           F2DA },
          {     0X5F00,        X_,            F2X },
               
          /* CALR */
          {     0XD000,        RANEG_,        F5 },

          /* CLR */
          {     0X8D80,        R_,            F2R },
          {     0X0D80,        IR_,           F2R },
          {     0X4D80,        DA_,           F2DA },
          {     0X4D80,        X_,            F2X },

          /* CLRB */
          {     0X8C80,        RB_,           F2R },
          {     0X0C80,        IR_,           F2R },
          {     0X4C80,        DA_,           F2DA },
          {     0X4C80,        X_,            F2X },

          /* COM */
          {     0X8D00,        R_,            F2R },
          {     0X0D00,        IR_,           F2R },
          {     0X4D00,        DA_,           F2DA },
          {     0X4D00,        X_,            F2X },

          /* COMB */
          {     0X8C00,        RB_,           F2R },
          {     0X0C00,        IR_,           F2R },
          {     0X4C00,        DA_,           F2DA },
          {     0X4C00,        X_,            F2X },

          /* COMFLG */
          {     0X8D50,        F_,            F2R },
          {     0X8D50,        F_F,           F2R2 },
          {     0X8D50,        F_F_F,         F2R3 },
          {     0X8D50,        F_F_F_F,       F2R4 },

          /* CP */
          {     0X8B00,        R_R,           F0R },
          {     0X0B00,        R_IR,          F0R },
          {     0X4B00,        R_DA,          F0DA },
          {     0X4B00,        R_X,           F0X },
          {     0X0B00,        R_IM,          F0IMW },
          {     0X0D10,        IR_IM,         F3RW },
          {     0X4D10,        DA_IM,         F3DAW },
          {     0X4D10,        X_IM,          F3XW },

          /* CPB */
          {     0X8A00,        RB_RB,         F0R },
          {     0X0A00,        RB_IR,         F0R },
          {     0X4A00,        RB_DA,         F0DA },
          {     0X4A00,        RB_X,          F0X },
          {     0X0A00,        RB_IM,         F0IMB },
          {     0X0C10,        IR_IM,         F3RB },
          {     0X4C10,        DA_IM,         F3DAB },
          {     0X4C10,        X_IM,          F3XB },

          /* CPD */
          {     0XBB80,        R_IR_R_CC,     F22 },

          /* CPDB */
          {     0XBA80,        RB_IR_R_CC,    F22 },

          /* CPDR */
          {     0XBBC0,        R_IR_R_CC,     F22 },

          /* CPDRB */
          {     0XBAC0,        RB_IR_R_CC,    F22 },

          /* CPI */
          {     0XBB00,        R_IR_R_CC,     F22 },

          /* CPIB */
          {     0XBA00,        RB_IR_R_CC,    F22 },

          /* CPIR */
          {     0XBB40,        R_IR_R_CC,     F22 },

          /* CPIRB */
          {     0XBA40,        RB_IR_R_CC,    F22 },

          /* CPL */
          {     0X9000,        RR_RR,         F0R },
          {     0X1000,        RR_IR,         F0R },
          {     0X5000,        RR_DA,         F0DA },
          {     0X5000,        RR_X,          F0X },
          {     0X1000,        RR_IM,         F0IML },

          /* CPSD */
          {     0XBBA0,        IR_IR_R_CC,    F22 },

          /* CPSDB */
          {     0XBAA0,        IR_IR_R_CC,    F22 },

          /* CPSDR */
          {     0XBBE0,        IR_IR_R_CC,    F22 },

          /* CPSDRB */
          {     0XBAE0,        IR_IR_R_CC,    F22 },

          /* CPSI */
          {     0XBB20,        IR_IR_R_CC,    F22 },

          /* CPSIB */
          {     0XBA20,        IR_IR_R_CC,    F22 },

          /* CPSIR */
          {     0XBB60,        IR_IR_R_CC,    F22 },

          /* CPSIRB */
          {     0XBA60,        IR_IR_R_CC,    F22 },

          /* DAB */
          {     0XB000,        RB_,           F2R },

          /* DBJNZ */         
          {     0XF000,        RB_RANEG,      F6 },

          /* DEC */
          {     0XAB00,        R_IM,          F1RM1 },
          {     0X2B00,        IR_IM,         F1RM1 },
          {     0X6B00,        DA_IM,         F1DAM1 },
          {     0X6B00,        X_IM,          F1XM1 },
          {     0XAB00,        R_,            F2R },
          {     0X2B00,        IR_,           F2R },
          {     0X6B00,        DA_,           F2DA },
          {     0X6B00,        X_,            F2X },

          /* DECB */
          {     0XAA00,        RB_IM,         F1RM1 },
          {     0X2A00,        IR_IM,         F1RM1 },
          {     0X6A00,        DA_IM,         F1DAM1 },
          {     0X6A00,        X_IM,          F1XM1 },
          {     0XAA00,        RB_,           F2R },
          {     0X2A00,        IR_,           F2R },
          {     0X6A00,        DA_,           F2DA },
          {     0X6A00,        X_,            F2X },

          /* DI */
          {     0X7C00,        INT_,          F21 },
          {     0X7C00,        INT_INT,       F212 },

          /* DIV */
          {     0X9B00,        RR_R,          F0R },
          {     0X1B00,        RR_IR,         F0R },
          {     0X5B00,        RR_DA,         F0DA },
          {     0X5B00,        RR_X,          F0X },
          {     0X1B00,        RR_IM,         F0IMW },

          /* DIVL */
          {     0X9A00,        RQ_RR,         F0R },
          {     0X1A00,        RQ_IR,         F0R },
          {     0X5A00,        RQ_DA,         F0DA },
          {     0X5A00,        RQ_X,          F0X },
          {     0X1A00,        RQ_IM,         F0IML },

          /* DJNZ */
          {     0XF080,        R_RANEG,       F6 },

          /* EI */
          {     0X7C80,        INT_,          F21 },
          {     0X7C80,        INT_INT,       F212 },
               
          /* EX */
          {     0XAD00,        R_R,           F0R },
          {     0X2D00,        R_IR,          F0R },
          {     0X6D00,        R_DA,          F0DA },
          {     0X6D00,        R_X,           F0X },

          /* EXB */
          {     0XAC00,        RB_RB,         F0R },
          {     0X2C00,        RB_IR,         F0R },
          {     0X6C00,        RB_DA,         F0DA },
          {     0X6C00,        RB_X,          F0X },
          
          /* EXTS */
          {     0XB1A0,        RR_,           F2R },

          /* EXTSB */
          {     0XB100,        R_,            F2R },

          /* EXTSL */
          {     0XB170,        RQ_,           F2R },

	  /* FADDD */
	  {	0X8E21,		RQ_RQ,		FDD },
 
	  /* FADDF */
	  {	0X8E01,		RR_RR,		FFF },
 
	  /* FCPD */
	  {	0X8E26,		RQ_RQ,		FDD },
 
	  /* FCPF */
	  {	0X8E06,		RR_RR,		FFF },
 
	  /* FDIVD */
	  {	0X8E24,		RQ_RQ,		FDD },
 
	  /* FDIVF */
	  {	0X8E04,		RR_RR,		FFF },
 
	  /* FDTOD */
	  {	0X8E1F,		RQ_RQ,		FDD },

	  /* FDTOF */
	  {	0X8E1B,		RR_RQ,		FFD },

	  /* FDTOI */
	  {	0X8E13,		R_RQ,		FID },

	  /* FDTOL */
	  {	0X8E17,		RR_RQ,		FFD },

	  /* FFTOD */
	  {	0X8E1E,		RQ_RR,		FDF },

	  /* FFTOF */
	  {	0X8E1A,		RR_RR,		FFF },

	  /* FFTOI */
	  {	0X8E12,		R_RR,		FIF },

	  /* FFTOL */
	  {	0X8E16,		RR_RR,		FFF },

	  /* FITOD */
	  {	0X8E1C,		RQ_R,		FDI },

	  /* FITOF */
	  {	0X8E18,		RR_R,		FFI },

	  /* FITOI */
	  {	0X8E10,		R_R,		FII },

	  /* FITOL */
	  {	0X8E14,		RR_R,		FFI },

	  /* FLTOD */
	  {	0X8E1D,		RQ_RR,		FDF },

	  /* FLTOF */
	  {	0X8E19,		RR_RR,		FFF },

	  /* FLTOI */
	  {	0X8E11,		R_RR,		FIF },

	  /* FLTOL */
	  {	0X8E15,		RR_RR,		FFF },

	  /* FMULD */
	  {	0X8E23,		RQ_RQ,		FDD },
 
	  /* FMULF */
	  {	0X8E03,		RR_RR,		FFF },
 
	  /* FNEGD */
	  {	0X8E25,		RQ_,		FD },
 
	  /* FNEGF */
	  {	0X8E05,		RR_,		FF },
 
	  /* FSUBD */
	  {	0X8E22,		RQ_RQ,		FDD },
 
	  /* FSUBF */
	  {	0X8E02,		RR_RR,		FFF },
 
          /* HALT */
          {     0X7A00,        NIL_,          F20 },

          /* IN */
          {     0X3D00,        R_IOR,         F0R },
          {     0X3B40,        R_DA,          F25 },

          /* INB */ 
          {     0X3C00,        RB_IOR,        F0R },
          {     0X3A40,        RB_DA,         F25 },

          /* INC */
          {     0XA900,        R_IM,          F1RM1 },
          {     0X2900,        IR_IM,         F1RM1 },
          {     0X6900,        DA_IM,         F1DAM1 },
          {     0X6900,        X_IM,          F1XM1 },
          {     0XA900,        R_,            F2R },
          {     0X2900,        IR_,           F2R },
          {     0X6900,        DA_,           F2DA },
          {     0X6900,        X_,            F2X },
               
          /* INCB */
          {     0XA800,        RB_IM,         F1RM1 },
          {     0X2800,        IR_IM,         F1RM1 },
          {     0X6800,        DA_IM,         F1DAM1 },
          {     0X6800,        X_IM,          F1XM1 },
          {     0XA800,        RB_,           F2R },
          {     0X2800,        IR_,           F2R },
          {     0X6800,        DA_,           F2DA },
          {     0X6800,        X_,            F2X },

          /* IND */
          {     0X3B88,        IR_IOR_R,      F24 },

          /* INDB */
          {     0X3A88,        IR_IOR_R,      F24 },

          /* INDR */
          {     0X3B80,        IR_IOR_R,      F24 },

          /* INDRB */
          {     0X3A80,        IR_IOR_R,      F24 },

          /* INI */
          {     0X3B08,        IR_IOR_R,      F24 },

          /* INIB */
          {     0X3A08,        IR_IOR_R,      F24 },

          /* INIR */
          {     0X3B00,        IR_IOR_R,      F24 },

          /* INIRB */
          {     0X3A00,        IR_IOR_R,      F24 },

          /* IRET */
          {     0X7B00,        NIL_,          F20 },

          /* JP */
          {     0X1E00,        CC_IR,         F0R },
          {     0X5E00,        CC_DA,         F0DA },
          {     0X5E00,        CC_X,          F0X },
          {     0X1E80,        IR_,           F2R },
          {     0X5E80,        DA_,           F2DA },
          {     0X5E80,        X_,            F2X },

          /* JPR */
          {     0X1E00,        CC_IR,         F0R },
          {     0X5E00,        CC_DA,         F0DA },
          {     0X5E00,        CC_X,          F0X },
          {     0X1E80,        IR_,           F2R },
          {     0X5E80,        DA_,           F2DA },
          {     0X5E80,        X_,            F2X },

          /* JR */
          {     0XE000,        CC_RA,         F4S },
          {     0XE800,        RA_,           F7S },

          /* LD */
          {     0XA100,        R_R,           F0R },
          {     0X2100,        R_IR,          F0R },
          {     0X6100,        R_DA,          F0DA },
          {     0X6100,        R_X,           F0X },
          {     0X2100,        R_IM,          F0IMW },
          {     0X3100,        R_BA,          F14 },
          {     0X7100,        R_BX,          F17 },
          {     0X2F00,        IR_R,          F1R },
          {     0X6F00,        DA_R,          F1DA },
          {     0X6F00,        X_R,           F1X },
          {     0X3300,        BA_R,          F16 },
          {     0X7300,        BX_R,          F18 },
          {     0X0D50,        IR_IM,         F3RW },
          {     0X4D50,        DA_IM,         F3DAW },
          {     0X4D50,        X_IM,          F3XW },
           
          /* LDA */
          {     0X7600,        SR_DA,         F0DA },
          {     0X7600,        SR_X,          F0X },
          {     0X3400,        SR_BA,         F14 },
          {     0X7400,        SR_BX,         F17 },

          /* LDAR */
          {     0X3400,        SR_RABYTE,     F13 },

          /* LDB */
          {     0XA000,        RB_RB,         F0R },
          {     0X2000,        RB_IR,         F0R },
          {     0X6000,        RB_DA,         F0DA },
          {     0X6000,        RB_X,          F0X },
          {     0XC000,        RB_IM,         F4 },
          {     0X3000,        RB_BA,         F14 },
          {     0X7000,        RB_BX,         F17 },
          {     0X2E00,        IR_RB,         F1R },
          {     0X6E00,        DA_RB,         F1DA },
          {     0X6E00,        X_RB,          F1X },
          {     0X3200,        BA_RB,         F16 },
          {     0X7200,        BX_RB,         F18 },
          {     0X0C50,        IR_IM,         F3RB },
          {     0X4C50,        DA_IM,         F3DAB },
          {     0X4C50,        X_IM,          F3XB },
          
          /* LDCTL */
          {     0X7D00,        R_CTLR,        F31 },
          {     0X7D80,        CTLR_R,        F32 },
          
          /* LDCTLB */
          {     0X8C90,        FLGR_RB,       F33 },
          {     0X8C10,        RB_FLGR,       F2R },

          /* LDD */
          {     0XBB98,        IR_IR_R,       F24 },

          /* LDDB */
          {     0XBA98,        IR_IR_R,       F24 },
 
          /* LDDR */
          {     0XBB90,        IR_IR_R,       F24 },
 
          /* LDDRB */
          {     0XBA90,        IR_IR_R,       F24 },
 
          /* LDI */
          {     0XBB18,        IR_IR_R,       F24 },
 
          /* LDIB */
          {     0XBA18,        IR_IR_R,       F24 },
 
          /* LDIR */
          {     0XBB10,        IR_IR_R,       F24 },
 
          /* LDIRB */
          {     0XBA10,        IR_IR_R,       F24 },
 
          /* LDK */
          {     0XBD00,        R_IM,          F1R },

          /* LDL */
          {     0X9400,        RR_RR,         F0R },
          {     0X1400,        RR_IR,         F0R },
          {     0X5400,        RR_DA,         F0DA },
          {     0X5400,        RR_X,          F0X },
          {     0X1400,        RR_IM,         F0IML },
          {     0X3500,        RR_BA,         F14 },
          {     0X7500,        RR_BX,         F17 },
          {     0X1D00,        IR_RR,         F1R },
          {     0X5D00,        DA_RR,         F1DA },
          {     0X5D00,        X_RR,          F1X },
          {     0X3700,        BA_RR,         F16 },
          {     0X7700,        BX_RR,         F18 },
 
          /* LDM */
          {     0X1C10,        R_IR_IM,       F8R },
          {     0X5C10,        R_DA_IM,       F8DA },
          {     0X5C10,        R_X_IM,        F8X },
          {     0X1C90,        IR_R_IM,       F9R },
          {     0X5C90,        DA_R_IM,       F9DA },
          {     0X5C90,        X_R_IM,        F9X },
 
          /* LDPS */
          {     0X3900,        IR_,           F2R },
          {     0X7900,        DA_,           F2DA },
          {     0X7900,        X_,            F2X },
 
          /* LDR */
          {     0X3100,        R_RABYTE,      F13 },
          {     0X3300,        RABYTE_R,      F15 },
 
          /* LDRB */
          {     0X3000,        RB_RA,         F13 },
          {     0X3200,        RA_RB,         F15 },
 
          /* LDRL */
          {     0X3500,        RR_RA,         F13 },
          {     0X3700,        RA_RR,         F15 },
 
          /* MBIT */
          {     0X7B0A,        NIL_,          F20 },
 
          /* MREQ */
          {     0X7BD0,        R_,            F2R },
 
          /* MRES */
          {     0X7B09,        NIL_,          F20 },
 
          /* MSET */
          {     0X7B08,        NIL_,          F20 },
 
          /* MULT */
          {     0X9900,        RR_R,          F0R },
          {     0X1900,        RR_IR,         F0R },
          {     0X5900,        RR_DA,         F0DA },
          {     0X5900,        RR_X,          F0X },
          {     0X1900,        RR_IM,         F0IMW },
 
          /* MULTL */
          {     0X9800,        RQ_RR,         F0R },
          {     0X1800,        RQ_IR,         F0R },
          {     0X5800,        RQ_DA,         F0DA },
          {     0X5800,        RQ_X,          F0X },
          {     0X1800,        RQ_IM,         F0IML },

          /* NEG */
          {     0X8D20,        R_,            F2R },
          {     0X0D20,        IR_,           F2R },
          {     0X4D20,        DA_,           F2DA },
          {     0X4D20,        X_,            F2X },
 
          /* NEGB */
          {     0X8C20,        RB_,           F2R },
          {     0X0C20,        IR_,           F2R },
          {     0X4C20,        DA_,           F2DA },
          {     0X4C20,        X_,            F2X },
 
          /* NOP */
          {     0X8D07,        NIL_,          F20 },
 
          /* OR */
          {     0X8500,        R_R,           F0R },
          {     0X0500,        R_IR,          F0R },
          {     0X4500,        R_DA,          F0DA },
          {     0X4500,        R_X,           F0X },
          {     0X0500,        R_IM,          F0IMW },
 
          /* ORB */
          {     0X8400,        RB_RB,         F0R },
          {     0X0400,        RB_IR,         F0R },
          {     0X4400,        RB_DA,         F0DA },
          {     0X4400,        RB_X,          F0X },
          {     0X0400,        RB_IM,         F0IMB },
 
          /* OTDR */
          {     0X3BA0,        IOR_IR_R,      F24 },
 
          /* OTDRB */
          {     0X3AA0,        IOR_IR_R,      F24 },
 
          /* OTIOR */
          {     0X3B20,        IOR_IR_R,      F24 },
 
          /* OTIORB */
          {     0X3A20,        IOR_IR_R,      F24 },
 
          /* OUT */
          {     0X3F00,        IOR_R,         F1R },
          {     0X3B60,        DA_R,          F26 },
 
          /* OUTB */
          {     0X3E00,        IOR_RB,        F1R },
          {     0X3A60,        DA_RB,         F26 },
 
          /* OUTD */
          {     0X3BA8,        IOR_IR_R,      F24 },
 
          /* OUTDB */
          {     0X3AA8,        IOR_IR_R,      F24 },
 
          /* OUTI */
          {     0X3B28,        IOR_IR_R,      F24 },
 
          /* OUTIB */
          {     0X3A28,        IOR_IR_R,      F24 },

          /* POP */
          {     0X9700,        R_IR,          F0R },
          {     0X1700,        IR_IR,         F0R },
          {     0X5700,        DA_IR,         F2X },
          {     0X5700,        X_IR,          F27 },
 
          /* POPL */
          {     0X9500,        RR_IR,         F0R },
          {     0X1500,        IR_IR,         F0R },
          {     0X5500,        DA_IR,         F2X },
          {     0X5500,        X_IR,          F27 },
 
          /* PUSH */
          {     0X9300,        IR_R,          F1R },
          {     0X1300,        IR_IR,         F1R },
          {     0X5300,        IR_DA,         F29 },
          {     0X5300,        IR_X,          F28 },
          {     0X0D90,        IR_IM,         F3RW },
 
          /* PUSHL */
 
          {     0X9100,        IR_RR,         F1R },
          {     0X1100,        IR_IR,         F1R },
          {     0X5100,        IR_DA,         F29 },
          {     0X5100,        IR_X,          F28 },
 
          /* RES */
          {     0XA300,        R_IM,          F1R },
          {     0X2300,        IR_IM,         F1R },
          {     0X6300,        DA_IM,         F1DA },
          {     0X6300,        X_IM,          F1X },
          {     0X2300,        R_R,           F12 },
 
          /* RESB */
          {     0XA200,        RB_IM,         F1R },
          {     0X2200,        IR_IM,         F1R },
          {     0X6200,        DA_IM,         F1DA },
          {     0X6200,        X_IM,          F1X },
          {     0X2200,        RB_R,          F12 },
 
          /* RESFLG */
          {     0X8D30,        F_,            F2R },
          {     0X8D30,        F_F,           F2R2 },
          {     0X8D30,        F_F_F,         F2R3 },
          {     0X8D30,        F_F_F_F,       F2R4 },
 
          /* RET */
          {     0X9E80,        NIL_,          F34 },
          {     0X9E00,        CC_,           F19 },
 
          /* RL */
          {     0XB300,        R_IM,          F30 },
          {     0XB300,        R_,            F30D },
 
          /* RLB */
          {     0XB200,        RB_IM,         F30 },
          {     0XB200,        RB_,           F30D },
 
          /* RLC */
          {     0XB380,        R_IM,          F30 },
          {     0XB380,        R_,            F30D },
 
          /* RLCB */
          {     0XB280,        RB_IM,         F30 },
          {     0XB280,        RB_,           F30D },
 
          /* RLDB */
          {     0XBE00,        RB_RB,         F0R },
 
          /* RR */
          {     0XB340,        R_IM,          F30 },
          {     0XB340,        R_,            F30D },
 
          /* RRB */
          {     0XB240,        RB_IM,         F30 },
          {     0XB240,        RB_,           F30D },
 
          /* RRC */
          {     0XB3C0,        R_IM,          F30 },
          {     0XB3C0,        R_,            F30D },
 
          /* RRCB */
          {     0XB2C0,        RB_IM,         F30 },
          {     0XB2C0,        RB_,           F30D },
 
          /* RRDB */
          {     0XBC00,        RB_RB,         F0R },

          /* SBC */
          {     0XB700,        R_R,           F0R },
 
          /* SBCB */
          {     0XB600,        RB_RB,         F0R },
 
          /* SC */
          {     0X7F00,        IM_,           F7 },
 
          /* SDA */
          {     0XB3B0,        R_R,           F10 },
 
          /* SDAB */
          {     0XB2B0,        RB_R,          F10 },
 
          /* SDAL */
          {     0XB3F0,        RR_R,          F10 },
 
          /* SDL */
          {     0XB330,        R_R,           F10 },
 
          /* SDLB */
          {     0XB230,        RB_R,          F10 },
 
          /* SDLL */
          {     0XB370,        RR_R,          F10 },
 
          /* SET */
          {     0XA500,        R_IM,          F1R },
          {     0X2500,        IR_IM,         F1R },
          {     0X6500,        DA_IM,         F1DA },
          {     0X6500,        X_IM,          F1X },
          {     0X2500,        R_R,           F12 },
 
          /* SETB */
          {     0XA400,        RB_IM,         F1R },
          {     0X2400,        IR_IM,         F1R },
          {     0X6400,        DA_IM,         F1DA },
          {     0X6400,        X_IM,          F1X },
          {     0X2400,        RB_R,          F12 },
 
          /* SETFLG */
          {     0X8D10,        F_,            F2R },
          {     0X8D10,        F_F,           F2R2 },
          {     0X8D10,        F_F_F,         F2R3 },
          {     0X8D10,        F_F_F_F,       F2R4 },
 
          /* SIN */
          {     0X3B50,        R_DA,          F25 },
 
          /* SINB */
          {     0X3A50,        RB_DA,         F25 },
 
          /* SIND */
          {     0X3B98,        IR_IOR_R,      F24 },
 
          /* SINDB */
          {     0X3A98,        IR_IOR_R,      F24 },
 
          /* SINDR */
          {     0X3B90,        IR_IOR_R,      F24 },
 
          /* SINDRB */
          {     0X3A90,        IR_IOR_R,      F24 },
 
          /* SINI */
          {     0X3B18,        IR_IOR_R,      F24 },
 
          /* SINIB */
          {     0X3A18,        IR_IOR_R,      F24 },
 
          /* SINIR */
          {     0X3B10,        IR_IOR_R,      F24 },
 
          /* SINIRB */
          {     0X3A10,        IR_IOR_R,      F24 },

          /* SLA */
          {     0XB390,        R_IM,          F11W },
          {     0XB390,        R_,            F11D },
 
          /* SLAB */
          {     0XB290,        RB_IM,         F11B },
          {     0XB290,        RB_,           F11D },
 
          /* SLAL */
          {     0XB3D0,        RR_IM,         F11L },
          {     0XB3D0,        RR_,           F11D },
 
          /* SLL */
          {     0XB310,        R_IM,          F11W },
          {     0XB310,        R_,            F11D },
 
          /* SLLB */
          {     0XB210,        RB_IM,         F11B },
          {     0XB210,        RB_,           F11D },
 
          /* SLLL */
          {     0XB350,        RR_IM,         F11L },
          {     0XB350,        RR_,           F11D },
 
          /* SOTDR */
          {     0X3BB0,        IOR_IR_R,       F24 },
 
          /* SOTDRB */       
          {     0X3AB0,        IOR_IR_R,       F24 },
 
          /* SOTIR */
          {     0X3B30,        IOR_IR_R,       F24 },
 
          /* SOTIRB */
          {     0X3A30,        IOR_IR_R,       F24 },

          /* SOUT */
          {     0X3B70,        DA_R,          F26 },
 
          /* SOUTB */
          {     0X3A70,        DA_RB,         F26 },
 
          /* SOUTD */
          {     0X3BB8,        IOR_IR_R,       F24 },
 
          /* SOUTDB */
          {     0X3AB8,        IOR_IR_R,       F24 },
 
          /* SOUTI */
          {     0X3B38,        IOR_IR_R,       F24 },
 
          /* SOUTIB */
          {     0X3A38,        IOR_IR_R,       F24 },

          /* SRA */
          {     0XB398,        R_IM,          F11W2 },
          {     0XB398,        R_,            F11D2 },
 
          /* SRAB */
          {     0XB298,        RB_IM,         F11B2 },
          {     0XB298,        RB_,           F11D2 },
 
          /* SRAL */
          {     0XB3D8,        RR_IM,         F11L2 },
          {     0XB3D8,        RR_,           F11D2 },
 
          /* SRL */
          {     0XB318,        R_IM,          F11W2 },
          {     0XB318,        R_,            F11D2 },
 
          /* SRLB */
          {     0XB218,        RB_IM,         F11B2 },
          {     0XB218,        RB_,           F11D2 },
 
          /* SRLL */
          {     0XB358,        RR_IM,         F11L2 },
          {     0XB358,        RR_,           F11D2 },
 
          /* SUB */
          {     0X8300,        R_R,           F0R },
          {     0X0300,        R_IR,          F0R },
          {     0X4300,        R_DA,          F0DA },
          {     0X4300,        R_X,           F0X },
          {     0X0300,        R_IM,          F0IMW },
 
          /* SUBB */
          {     0X8200,        RB_RB,         F0R },
          {     0X0200,        RB_IR,         F0R },
          {     0X4200,        RB_DA,         F0DA },
          {     0X4200,        RB_X,          F0X },
          {     0X0200,        RB_IM,         F0IMB },
 
          /* SUBL */
          {     0X9200,        RR_RR,         F0R },
          {     0X1200,        RR_IR,         F0R },
          {     0X5200,        RR_DA,         F0DA },
          {     0X5200,        RR_X,          F0X },
          {     0X1200,        RR_IM,         F0IML },
 
          /* TCC */
          {     0XAF00,        CC_R,          F0R },
 
          /* TCCB */
          {     0XAE00,        CC_RB,         F0R },

          /* TEST */
          {     0X8D40,        R_,            F2R },
          {     0X0D40,        IR_,           F2R },
          {     0X4D40,        DA_,           F2DA },
          {     0X4D40,        X_,            F2X },
 
          /* TESTB */
          {     0X8C40,        RB_,           F2R },
          {     0X0C40,        IR_,           F2R },
          {     0X4C40,        DA_,           F2DA },
          {     0X4C40,        X_,            F2X },
 
          /* TESTL */
          {     0X9C80,        RR_,           F2R },
          {     0X1C80,        IR_,           F2R },
          {     0X5C80,        DA_,           F2DA },
          {     0X5C80,        X_,            F2X },
 
          /* TRDB */
          {     0XB880,        IR_IR_R,       F23 },
 
          /* TRDRB */
          {     0XB8C0,        IR_IR_R,       F23 },
 
          /* TRIB */
          {     0XB800,        IR_IR_R,       F23 },
 
          /* TRIRB */
          {     0XB840,        IR_IR_R,       F23 },
 
          /* TRTDB */
          {     0XB8A0,        IR_IR_R,       F23 },
 
          /* TRTDRB */
          {     0XB8E0,        IR_IR_R,       F35 },
 
          /* TRTIB */
          {     0XB820,        IR_IR_R,       F23 },
 
          /* TRTIRB */
          {     0XB860,        IR_IR_R,       F23 },
 
          /* TSET */
          {     0X8D60,        R_,            F2R },
          {     0X0D60,        IR_,           F2R },
          {     0X4D60,        DA_,           F2DA },
          {     0X4D60,        X_,            F2X },
 
          /* TSETB */
          {     0X8C60,        RB_,           F2R },
          {     0X0C60,        IR_,           F2R },
          {     0X4C60,        DA_,           F2DA },
          {     0X4C60,        X_,            F2X },
 
          /* XOR */
          {     0X8900,        R_R,           F0R },
          {     0X0900,        R_IR,          F0R },
          {     0X4900,        R_DA,          F0DA },
          {     0X4900,        R_X,           F0X },
          {     0X0900,        R_IM,          F0IMW },
 
          /* XORB */
          {     0X8800,        RB_RB,         F0R },
          {     0X0800,        RB_IR,         F0R },
          {     0X4800,        RB_DA,         F0DA },
          {     0X4800,        RB_X,          F0X },
          {     0X0800,        RB_IM,         F0IMB },
     };
