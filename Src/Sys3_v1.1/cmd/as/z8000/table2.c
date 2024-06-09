#
/* WARNING *******************************************************/
/*                                                               */
/*   DO NOT ADD ANY NEW ENTRIES IN THIS TABLE WITHOUT ADDING    */
/*   A CORRESPONDING CONSTANT TO THE FILE "index.h".          */
/*                                                               */
/*****************************************************************/


#include <stdio.h>
#include "common.h"
#include "class.h"
#include "opcode.h"
#include "operand.h"
#include "symtab.h"
#include "types.h"


char operandtable [] [2] = {
 
	{ NILOP,      NILOP },
 
	{ R,          NILOP },
	{ RB,         NILOP },
	{ RR,         NILOP },
	{ RQ,         NILOP },
	{ IR,         NILOP },
	{ DA,         NILOP },
	{ X,          NILOP },
	{ IM,         NILOP },
	{ RA,         NILOP },
	{ RANEG,      NILOP },
	{ CC,         NILOP },
	{ FLAG,       NILOP },
	{ INT,        NILOP },
 
	{ R,          R },
	{ R,          RB },
	{ R,          RR },
	{ R,          RQ },
	{ R,          IR },
        { R,          IOR },

	{ R,          DA },
	{ R,          X },
	{ R,          IM },
	{ R,          RA },
	{ R,          RABYTE },
	{ RABYTE,     R },
	{ R,          RANEG },
	{ R,          BA },
	{ R,          BX },
	{ R,          CTLR },
 
        { SR,         DA },
        { SR,         X },
        { SR,         BA },
        { SR,         BX },
        { SR,         RABYTE },

	{ RB,         R },
	{ RB,         RB },
	{ RB,         IR },
	{ RB,         IOR },
	{ RB,         DA },
	{ RB,         X },
	{ RB,         IM },
	{ RB,         RA },
	{ RB,         RANEG },
	{ RB,         BA },
	{ RB,         BX },
	{ RB,         FLGR },
 
	{ RR,         R },
	{ RR,         RB },
	{ RR,         RR },
	{ RR,         RQ },
	{ RR,         IR },
	{ RR,         DA },
	{ RR,         X },
	{ RR,         IM },
	{ RR,         RA },
	{ RR,         BA },
	{ RR,         BX },
 
	{ RQ,         R },
	{ RQ,         RR },
	{ RQ,         RQ },
	{ RQ,         IR },
	{ RQ,         DA },
	{ RQ,         X },
	{ RQ,         IM },
 
	{ IR,         R },
	{ IR,         RR },
	{ IR,         RB },
	{ IR,         IR },
	{ IR,         DA },
	{ IR,         X },
	{ IR,         IM },
 
        { IOR,        R },
        { IOR,        RB },

	{ DA,         R },
	{ DA,         RB },
	{ DA,         RR },
	{ DA,         IR },
	{ DA,         IM },
 
	{ X,          R },
	{ X,          RB },
	{ X,          RR },
	{ X,          IR },
	{ X,          IM },
 
	{ RA,         R },
	{ RA,         RB },
	{ RA,         RR },

	{ BA,         R },
	{ BA,         RB },
	{ BA,         RR },
 
	{ BX,         R },
	{ BX,         RB },
	{ BX,         RR },
 
	{ CC,         R },
	{ CC,         RB },
	{ CC,         IR },
	{ CC,         DA },
	{ CC,         X },
	{ CC,         RA },

	{ FLAG,       FLAG },

	{ INT,        INT },

	{ CTLR,       R },
 
	{ FLGR,       R },
	{ FLGR,       RB },

	{ R,          CON },
	{ IR,         IM },
 
	{ R,          CON },
	{ DA,         IM },
 
	{ R,          CON },
	{ X,          IM },
 
	{ IR,         CON },
	{ R,          IM },
 
	{ IR,         CON },
	{ IR,         R },
 
        { IR,         CON },
        { IOR,        R },

        { IOR,        CON },
        { IR,         R },

	{ DA,         CON },
	{ R,          IM },
 
	{ X,          CON },
	{ R,          IM },
 
	{ FLAG,       CON },
	{ FLAG,       FLAG },

	{ R,          CON },
	{ IR,         CON },
	{ R,          CC },
 
	{ RB,         CON },
	{ IR,         CON },
	{ R,          CC },

	{ IR,         CON },
	{ IR,         CON },
	{ R,          CC },

	{ FLAG,       CON },
	{ FLAG,       CON },
	{ FLAG,       FLAG },
     };
                                                       


/* WARNING *******************************************************/
/*                                                               */
/*   DO NOT CHANGE THIS TABLE WITHOUT CHANGING THE VALUES OF     */
/*   THE CONSTANTS IN THE FILE "register.h".              */
/*                                                               */
/*****************************************************************/


char classtable [CLASSTABLESIZE] [MAXCLASS] = {
 
	/* R    */   { SREG,		REG },
	/* RB   */   { REGHILO,		REGHILO },
	/* RR   */   { REGPAIR,		REGPAIR },
	/* RQ   */   { REGQUAD,		REGQUAD },
	/* IR   */   { INDREG,		INDREGPAIR},
	/* DA   */   { IDENT,		IDENT },
	/* X    */   { INDEXED,		INDEXED },
	/* IM   */   { IMMED,		IMMED },
	/* CC   */   { CNDCODE,		CNDCODE },
	/* RA   */   { IDENT,		IDENT },
	/* RABYTE */ { IDENT,		IDENT },
	/* RANEG */  { IDENT,		IDENT },
	/* BA   */   { BADDR,		BADDR },
	/* BX   */   { BINDEX,		BINDEX },
	/* CTLR */   { CTLREG,		CTLREG },
	/* FLGR */   { FLAGREG,		FLAGREG },
	/* INT  */   { INTRPT,		INTRPT },
	/* FLAG */   { CNDCODE,		CNDCODE },
	/* NILOP */  { NUL,		NUL },
	/* IOR */    { IOREGISTER,	INDREG },
	/* SR */     { REG,		SREGPAIR },
     };
                           

/* WARNING *******************************************************/
/*                                                               */
/*   DO CHANGE THIS TABLE WITHOUT CHANGING THE CORRESPONDING     */
/*   CONSTANT IN THE FILE "format.h".                         */
/*                                                               */
/*****************************************************************/


FORMATENTRY formattable [] = {
 
{  0, {     0,     0,      0,         0,      0 } },
{  2, { OPCB1,  OP2N,   OP1N,       NUL,    NUL } },   /* F0R */
{  4, { OPCB1,  N0,     OP1N,       OP2AW,  NUL } },   /* F0DA */
{  4, { OPCB1,  OP3N,   OP1N,       OP2AW,  NUL } },   /* F0X */
{  4, { OPCB1,  N0,     OP1N,       OP2W,   NUL } },   /* F0IMW */
{  4, { OPCB1,  N0,     OP1N,       OP2B,   OP2B } },  /* F0IMB */
{  6, { OPCB1,  N0,     OP1N,       OP2L,   NUL } },   /* F0IML */
 
{  2, { OPCB1,  OP1N,   OP2N,       NUL,    NUL } },   /* F1R */
{  4, { OPCB1,  N0,     OP2N,       OP1AW,  NUL } },   /* F1DA */
{  4, { OPCB1,  OP2N,   OP3N,       OP1AW,  NUL } },   /* F1X */
                                      
{  2, { OPCB1,  OP1N,   OP2NM1,     NUL,    NUL } },   /* F1RM1 */
{  4, { OPCB1,  N0,     OP2NM1,     OP1AW,  NUL } },   /* F1DAM1 */
{  4, { OPCB1,  OP2N,   OP3NM1,     OP1AW,  NUL } },   /* F1XM1 */

{  2, { OPCB1,  OP1N,   OPCN3,      NUL,    NUL } },   /* F2R */
{  2, { OPCB1,  OR2,    OPCN3,      NUL,    NUL } },   /* F2R2 */
{  2, { OPCB1,  OR3,    OPCN3,      NUL,    NUL } },   /* F2R3 */
{  2, { OPCB1,  OR4,    OPCN3,      NUL,    NUL } },   /* F2R4 */
{  4, { OPCB1,  N0,     OPCN3,      OP1AW,  NUL } },   /* F2DA */
{  4, { OPCB1,  OP2N,   OPCN3,      OP1AW,  NUL } },   /* F2X */
 
{  4, { OPCB1,  OP1N,   OPCN3,      OP2W,   NUL } },   /* F3RW */
{  4, { OPCB1,  OP1N,   OPCN3,      OP2B,   OP2B } },  /* F3RB */
{  6, { OPCB1,  OP1N,   OPCN3,      OP2L,   NUL } },   /* F3RL */
{  6, { OPCB1,  N0,     OPCN3,      OP1AW,  OP2W } },  /* F3DAW */
{  6, { OPCB1,  N0,     OPCN3,      OP1AW,  CON } },   /* F3DAB */
{  0, { OP2B,   OP2B,   NUL,        NUL,    NUL } },   /* F3DAB CONT */
{  8, { OPCB1,  N0,     OPCN3,      OP1AW,  OP2L } },  /* F3DAL */
{  6, { OPCB1,  OP2N,   OPCN3,      OP1AW,  OP3W } },  /* F3XW */
{  6, { OPCB1,  OP2N,   OPCN3,      OP1AW,  CON } },   /* F3XB */
{  0, { OP3B,   OP3B,   NUL,        NUL,    NUL } },   /* F3XB CONT */
{  8, { OPCB1,  OP2N,   OPCN3,      OP1AW,  OP3L } },  /* F3XL */
 
{  2, { OPCN1,  OP1N,   OP2B,       NUL,    NUL } },   /* F4 */
{  2, { OPCN1,  OP1SC,  NUL,        NUL,    NUL } },   /* F5 */
{  2, { OPCN1,  OP1N,   OPCBT7,     OP27BTS,NUL } },   /* F6 */
{  2, { OPCB1,  OP1B,   NUL,        NUL,    NUL } },   /* F7 */

{  4, { OPCB1,  OP2N,   OPCN3,      N0,     CON } },   /* F8R */
{  0, { OP1N,   N0,     OP3NM1,     NUL,    NUL } },   /* F8R CONT */
{  6, { OPCB1,  N0,     OPCN3,      N0,     CON } },   /* F8DA */
{  0, { OP1N,   N0,     OP3NM1,     OP2AW,  NUL } },   /* F8DA CONT */
{  6, { OPCB1,  OP3N,   OPCN3,      N0,     CON } },   /* F8X */
{  0, { OP1N,   N0,     OP4NM1,     OP2AW,  NUL } },   /* F8X CONT */
 
{  4, { OPCB1,  OP1N,   OPCN3,      N0,     CON } },   /* F9R */
{  0, { OP2N,   N0,     OP3NM1,     NUL,    NUL } },   /* F9R CONT */
{  6, { OPCB1,  N0,     OPCN3,      N0,     CON } },   /* F9DA */
{  0, { OP2N,   N0,     OP3NM1,     OP1AW,  NUL } },   /* F9DA CONT */
{  6, { OPCB1,  OP2N,   OPCN3,      N0,     CON } },   /* F9X */
{  0, { OP3N,   N0,     OP4NM1,     OP1AW,  NUL } },   /* F9X CONT */
 
{  4, { OPCB1,  OP1N,   OPCN3,      N0,     CON } },   /* F10 */
{  0, { OP2N,   B0,     NUL,        NUL,    NUL } },   /* F10 CONT */

{  4, { OPCB1,  OP1N,   OPCN3,      WVALUE1,NUL } },   /* F11D */
{  4, { OPCB1,  OP1N,   OPCN3,      OP2W,   NUL } },   /* F11B */
{  4, { OPCB1,  OP1N,   OPCN3,      OP2W,   NUL } },   /* F11W */
{  4, { OPCB1,  OP1N,   OPCN3,      OP2W,   NUL } },   /* F11L */
{  4, { OPCB1,  OP1N,   OPCN3,      B1,     B1  } },   /* F11D2 */
{  4, { OPCB1,  OP1N,   OPCN3,      OP2W2,  NUL } },   /* F11B2 */
{  4, { OPCB1,  OP1N,   OPCN3,      OP2W2,  NUL } },   /* F11W2 */
{  4, { OPCB1,  OP1N,   OPCN3,      OP2W2,  NUL } },   /* F11L2 */
 
{  4, { OPCB1,  N0,     OP2N,       N0,     CON } },   /* F12 */
{  0, { OP1N,   B0,     NUL,        NUL,    NUL } },   /* F12 CONT */
 
{  4, { OPCB1,  N0,     OP1N,       OP2W,   NUL } },   /* F13 */
 
{  4, { OPCB1,  OP2N,   OP1N,       OP3W,   NUL } },   /* F14 */

{  4, { OPCB1,  N0,     OP2N,       OP1W,   NUL } },   /* F15 */
 
{  4, { OPCB1,  OP1N,   OP3N,       OP2W,   NUL } },   /* F16 */
 
{  4, { OPCB1,  OP2N,   OP1N,       N0,     CON } },   /* F17 */
{  0, { OP3N,   B0,     NUL,        NUL,    NUL } },   /* F17 CONT */
 
{  4, { OPCB1,  OP1N,   OP3N,       N0,     CON } },   /* F18 */
{  0, { OP2N,   B0,     NUL,        NUL,    NUL } },   /* F18 CONT */
 
{  2, { OPCB1,  N0,     OP1N,       NUL,    NUL } },   /* F19 */
 
{  2, { OPCW,   NUL,    NUL,        NUL,    NUL } },   /* F20 */
 
{  2, { OPCB1,  N0,     BT0,        OPCBT7, OP12BTS} },/* F21 */
{  2, { OPCB1,  N0,     BT0,        OPCBT7, AND22BTS}},/* F212 */
 
{  4, { OPCB1,  OP2N,   OPCN3,      N0,     CON } },   /* F22 */
{  0, { OP3N,   OP1N,   OP4N,       NUL,    NUL } },   /* F22 CONT */

{  4, { OPCB1,  OP1N,   OPCN3,      N0,     CON } },   /* F23 */
{  0, { OP3N,   OP2N,   N0,         NUL,    NUL } },   /* F23 CONT */

{  4, { OPCB1,  OP2N,   OPCN3,      N0,     CON } },   /* F24 */
{  0, { OP3N,   OP1N,   OPCN4,      NUL,    NUL } },   /* F24 CONT */
 
{  4, { OPCB1,  OP1N,   OPCN3,      OP2W,   NUL } },   /* F25 */

{  4, { OPCB1,  OP2N,   OPCN3,      OP1W,   NUL } },   /* F26 */
 
{  4, { OPCB1,  OP3N,   OP2N,       OP1AW,  NUL } },   /* F27 */
 
{  4, { OPCB1,  OP1N,   OP3N,       OP2AW,  NUL } },   /* F28 */
 
{  4, { OPCB1,  OP1N,   N0,         OP2AW,  NUL } },   /* F29 */
 
{  2, { OPCB1,  OP1N,   OPCBTS67,   OP2BT1, BT0 } },   /* F30 */
{  2, { OPCB1,  OP1N,   OPCBTS67,   BT0,    BT0 } },   /* F30D */
 
{  2, { OPCB1,  OP1N,   OPCBT7,     OP23BTS,NUL } },   /* F31 */

{  2, { OPCB1,  OP2N,   OPCBT7,     OP13BTS,NUL } },   /* F32 */
 
{  2, { OPCB1,  OP2N,   OPCN3,      NUL,    NUL } },   /* F33 */

{  2, { OPCB1,  N0,     OPCN3,      NUL,    NUL } },   /* F34 */
 
{  4, { OPCB1,  OP1N,   OPCN3,      N0,     CON } },   /* F35 */
{  0, { OP3N,   OP2N,   N0E,        NUL,    NUL } },    /* F35 CONT */
{  2, { OPCN1,  OP1N,   OP2SB,      NUL,    NUL } },   /* F4S */
{  2, { OPCB1,  OP1SB,  NUL,        NUL,    NUL } },    /* F7S */
{  2, { OPCB1,	OP1FD,	OP1FD,      OPCBTS45,OPCN4}},    /* FD */
{  2, { OPCB1,	OP1FF,	OP1FF,      OPCBTS45,OPCN4}},    /* FF */
{  2, { OPCB1,  OP1FD,  OP2FD,      OPCBTS45,OPCN4}},	/* FDD */
{  2, { OPCB1,  OP1FD,  OP2FF,      OPCBTS45,OPCN4}},	/* FDF */
{  2, { OPCB1,  OP1FD,  OP2FI,      OPCBTS45,OPCN4}},	/* FDI */
{  2, { OPCB1,  OP1FF,  OP2FD,      OPCBTS45,OPCN4}},	/* FFD */
{  2, { OPCB1,  OP1FF,  OP2FF,      OPCBTS45,OPCN4}},	/* FFF */
{  2, { OPCB1,  OP1FF,  OP2FI,      OPCBTS45,OPCN4}},	/* FFI */
{  2, { OPCB1,  OP1FI,  OP2FD,      OPCBTS45,OPCN4}},	/* FID */
{  2, { OPCB1,  OP1FI,  OP2FF,      OPCBTS45,OPCN4}},	/* FIF */
{  2, { OPCB1,  OP1FI,  OP2FI,      OPCBTS45,OPCN4}},	/* FII */
};
