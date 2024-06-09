#define BYTE 8
#define PAIR(A,B) (A|(B<<BYTE))
/*
character name and code tables
default width tables
modified for BTL special font version 4
and Commercial II
*/

int chtab [] = {
PAIR('h','y'), 0200,	/*hyphen*/
PAIR('b','u'), 0201,	/*bullet*/
PAIR('s','q'), 0202,	/*square*/
PAIR('e','m'), 0203,	/*3/4em*/
PAIR('r','u'), 0204,	/*rule*/
PAIR('1','4'), 0205,	/*1/4*/
PAIR('1','2'), 0206,	/*1/2*/
PAIR('3','4'), 0207,	/*3/4*/
PAIR('m','i'), 0302,	/*equation minus*/
PAIR('f','i'), 0211,	/*fi*/
PAIR('f','l'), 0212,	/*fl*/
PAIR('f','f'), 0213,	/*ff*/
PAIR('F','i'), 0214,	/*ffi*/
PAIR('F','l'), 0215,	/*ffl*/
PAIR('d','e'), 0216,	/*degree*/
PAIR('d','g'), 0217,	/*dagger*/
PAIR('s','c'), 0220,	/*section*/
PAIR('f','m'), 0221,	/*foot mark*/
PAIR('a','a'), 0222,	/*acute accent*/
PAIR('g','a'), 0223,	/*grave accent*/
PAIR('u','l'), 0224,	/*underrule*/
PAIR('s','l'), 0225,	/*slash (longer)*/
PAIR('*','a'), 0230,	/*alpha*/
PAIR('*','b'), 0231,	/*beta*/
PAIR('*','g'), 0232,	/*gamma*/
PAIR('*','d'), 0233,	/*delta*/
PAIR('*','e'), 0234,	/*epsilon*/
PAIR('*','z'), 0235,	/*zeta*/
PAIR('*','y'), 0236,	/*eta*/
PAIR('*','h'), 0237,	/*theta*/
PAIR('*','i'), 0240,	/*iota*/
PAIR('*','k'), 0241,	/*kappa*/
PAIR('*','l'), 0242,	/*lambda*/
PAIR('*','m'), 0243,	/*mu*/
PAIR('*','n'), 0244,	/*nu*/
PAIR('*','c'), 0245,	/*xi*/
PAIR('*','o'), 0246,	/*omicron*/
PAIR('*','p'), 0247,	/*pi*/
PAIR('*','r'), 0250,	/*rho*/
PAIR('*','s'), 0251,	/*sigma*/
PAIR('*','t'), 0252,	/*tau*/
PAIR('*','u'), 0253,	/*upsilon*/
PAIR('*','f'), 0254,	/*phi*/
PAIR('*','x'), 0255,	/*chi*/
PAIR('*','q'), 0256,	/*psi*/
PAIR('*','w'), 0257,	/*omega*/
PAIR('*','A'), 0101,	/*Alpha*/
PAIR('*','B'), 0102,	/*Beta*/
PAIR('*','G'), 0260,	/*Gamma*/
PAIR('*','D'), 0261,	/*Delta*/
PAIR('*','E'), 0105,	/*Epsilon*/
PAIR('*','Z'), 0132,	/*Zeta*/
PAIR('*','Y'), 0110,	/*Eta*/
PAIR('*','H'), 0262,	/*Theta*/
PAIR('*','I'), 0111,	/*Iota*/
PAIR('*','K'), 0113,	/*Kappa*/
PAIR('*','L'), 0263,	/*Lambda*/
PAIR('*','M'), 0115,	/*Mu*/
PAIR('*','N'), 0116,	/*Nu*/
PAIR('*','C'), 0264,	/*Xi*/
PAIR('*','O'), 0117,	/*Omicron*/
PAIR('*','P'), 0265,	/*Pi*/
PAIR('*','R'), 0120,	/*Rho*/
PAIR('*','S'), 0266,	/*Sigma*/
PAIR('*','T'), 0124,	/*Tau*/
PAIR('*','U'), 0270,	/*Upsilon*/
PAIR('*','F'), 0271,	/*Phi*/
PAIR('*','X'), 0130,	/*Chi*/
PAIR('*','Q'), 0272,	/*Psi*/
PAIR('*','W'), 0273,	/*Omega*/
PAIR('s','r'), 0274,	/*square root*/
PAIR('t','s'), 0275,	/*terminal sigma*/
PAIR('r','n'), 0276,	/*root en*/
PAIR('>','='), 0277,	/*>=*/
PAIR('<','='), 0300,	/*<=*/
PAIR('=','='), 0301,	/*identically equal*/
PAIR('~','='), 0303,	/*approx =*/
PAIR('a','p'), 0304,	/*approximates*/
PAIR('!','='), 0305,	/*not equal*/
PAIR('-','>'), 0306,	/*right arrow*/
PAIR('<','-'), 0307,	/*left arrow*/
PAIR('u','a'), 0310,	/*up arrow*/
PAIR('d','a'), 0311,	/*down arrow*/
PAIR('e','q'), 0312,	/*equation equal*/
PAIR('m','u'), 0313,	/*multiply*/
PAIR('d','i'), 0314,	/*divide*/
PAIR('+','-'), 0315,	/*plus-minus*/
PAIR('c','u'), 0316,	/*cup (union)*/
PAIR('c','a'), 0317,	/*cap (intersection)*/
PAIR('s','b'), 0320,	/*subset of*/
PAIR('s','p'), 0321,	/*superset of*/
PAIR('i','b'), 0322,	/*improper subset*/
PAIR('i','p'), 0323,	/*  " superset*/
PAIR('i','f'), 0324,	/*infinity*/
PAIR('p','d'), 0325,	/*partial derivative*/
PAIR('g','r'), 0326,	/*gradient*/
PAIR('n','o'), 0327,	/*not*/
PAIR('i','s'), 0330,	/*integral sign*/
PAIR('p','t'), 0331,	/*proportional to*/
PAIR('e','s'), 0332,	/*empty set*/
PAIR('m','o'), 0333,	/*member of*/
PAIR('p','l'), 0334,	/*equation plus*/
PAIR('r','g'), 0335,	/*registered*/
PAIR('c','o'), 0336,	/*copyright*/
PAIR('b','r'), 0337,	/*box vert rule*/
PAIR('c','t'), 0340,	/*cent sign*/
PAIR('d','d'), 0341,	/*dbl dagger*/
PAIR('r','h'), 0342,	/*right hand*/
PAIR('l','h'), 0343,	/*left hand*/
PAIR('*','*'), 0344,	/*math * */
PAIR('b','s'), 0345,	/*bell system sign*/
PAIR('o','r'), 0346,	/*or*/
PAIR('c','i'), 0347,	/*circle*/
PAIR('l','t'), 0350,	/*left top (of big curly)*/
PAIR('l','b'), 0351,	/*left bottom*/
PAIR('r','t'), 0352,	/*right top*/
PAIR('r','b'), 0353,	/*right bot*/
PAIR('l','k'), 0354,	/*left center of big curly bracket*/
PAIR('r','k'), 0355,	/*right center of big curly bracket*/
PAIR('b','v'), 0356,	/*bold vertical*/
PAIR('l','f'), 0357,	/*left floor (left bot of big sq bract)*/
PAIR('r','f'), 0360,	/*right floor (rb of ")*/
PAIR('l','c'), 0361,	/*left ceiling (lt of ")*/
PAIR('r','c'), 0362,	/*right ceiling (rt of ")*/
0,0};

char codetab[256-32] = {	/*cat codes*/
00,	/*space*/
0145,	/*!*/
0230,	/*"*/
0337,	/*#*/
0155,	/*$*/
053,	/*%*/
050,	/*&*/
032,	/*' close*/
0132,	/*(*/
0133,	/*)*/
0122,	/***/
0143,	/*+*/
047,	/*,*/
040,	/*- hyphen*/
044,	/*.*/
043,	/*/*/
0110,	/*0*/
0111,	/*1*/
0112,	/*2*/
0113,	/*3*/
0114,	/*4*/
0115,	/*5*/
0116,	/*6*/
0117,	/*7*/
0120,	/*8*/
0121,	/*9*/
0142,	/*:*/
023,	/*;*/
0303,	/*<*/
0140,	/*=*/
0301,	/*>*/
0147,	/*?*/
0222,	/*@*/
0103,	/*A*/
075,	/*B*/
070,	/*C*/
074,	/*D*/
072,	/*E*/
0101,	/*F*/
065,	/*G*/
060,	/*H*/
066,	/*I*/
0105,	/*J*/
0107,	/*K*/
063,	/*L*/
062,	/*M*/
061,	/*N*/
057,	/*O*/
067,	/*P*/
055,	/*Q*/
064,	/*R*/
076,	/*S*/
056,	/*T*/
0106,	/*U*/
071,	/*V*/
0104,	/*W*/
0102,	/*X*/
077,	/*Y*/
073,	/*Z*/
0134,	/*[*/
0241,	/*\*/
0135,	/*]*/
0336,	/*^*/
0240,	/*_*/
030,	/*` open*/
025,	/*a*/
012,	/*b*/
027,	/*c*/
011,	/*d*/
031,	/*e*/
014,	/*f*/
045,	/*g*/
001,	/*h*/
006,	/*i*/
015,	/*j*/
017,	/*k*/
005,	/*l*/
004,	/*m*/
003,	/*n*/
033,	/*o*/
021,	/*p*/
042,	/*q*/
035,	/*r*/
010,	/*s*/
002,	/*t*/
016,	/*u*/
037,	/*v*/
041,	/*w*/
013,	/*x*/
051,	/*y*/
007,	/*z*/
0332,	/*{*/
0151,	/*|*/
0333,	/*}*/
0342,	/*~*/
00,	/*narrow space*/
040,	/*hyphen*/
0146,	/*bullet*/
0154,	/*square*/
022,	/*3/4 em*/
026,	/*rule*/
034,	/*1/4*/
036,	/*1/2*/
046,	/*3/4*/
0123,	/*minus*/
0124,	/*fi*/
0125,	/*fl*/
0126,	/*ff*/
0131,	/*ffi*/
0130,	/*ffl*/
0136,	/*degree*/
0137,	/*dagger*/
0355,	/*section*/
0150,	/*foot mark*/
0334,	/*acute accent*/
0335,	/*grave accent*/
0240,	/*underrule*/
0304,	/*slash (longer)*/
00,	/*half nar sp*/
00,	/**/
0225,	/*alpha*/
0212,	/*beta*/
0245,	/*gamma*/
0211,	/*delta*/
0231,	/*epsilon*/
0207,	/*zeta*/
0214,	/*eta*/
0202,	/*theta*/
0206,	/*iota*/
0217,	/*kappa*/
0205,	/*lambda*/
0204,	/*mu*/
0203,	/*nu*/
0213,	/*xi*/
0233,	/*omicron*/
0221,	/*pi*/
0235,	/*rho*/
0210,	/*sigma*/
0237,	/*tau*/
0216,	/*upsilon*/
0215,	/*phi*/
0227,	/*chi*/
0201,	/*psi*/
0251,	/*omega*/
0265,	/*Gamma*/
0274,	/*Delta*/
0256,	/*Theta*/
0263,	/*Lambda*/
0302,	/*Xi*/
0267,	/*Pi*/
0276,	/*Sigma*/
00,	/**/
0306,	/*Upsilon*/
0255,	/*Phi*/
0242,	/*Psi*/
0257,	/*Omega*/
0275,	/*square root*/
0262,	/*terminal sigma (was root em)*/
0261,	/*root en*/
0327,	/*>=*/
0326,	/*<=*/
0330,	/*identically equal*/
0264,	/*equation minus*/
0277,	/*approx =*/
0272,	/*approximates*/
0331,	/*not equal*/
0354,	/*right arrow*/
0234,	/*left arrow*/
0236,	/*up arrow*/
0223,	/*down arrow*/
0232,	/*equation equal*/
0323,	/*multiply*/
0324,	/*divide*/
0325,	/*plus-minus*/
0260,	/*cup (union)*/
0305,	/*cap (intersection)*/
0270,	/*subset of*/
0271,	/*superset of*/
0350,	/*improper subset*/
0246,	/* improper superset*/
0244,	/*infinity*/
0273,	/*partial derivative*/
0253,	/*gradient*/
0307,	/*not*/
0266,	/*integral sign*/
0247,	/*proportional to*/
0343,	/*empty set*/
0341,	/*member of*/
0353,	/*equation plus*/
0141,	/*registered*/
0153,	/*copyright*/
0346,	/*box rule (was parallel sign)*/
0127,	/*cent sign*/
0345,	/*dbl dagger*/
0250,	/*right hand*/
0340,	/*left hand*/
0347,	/*math * */
0243,	/*bell system sign*/
0226,	/*or (was star)*/
0351,	/*circle*/
0311,	/*left top (of big curly)*/
0314,	/*left bottom*/
0315,	/*right top*/
0317,	/*right bot*/
0313,	/*left center of big curly bracket*/
0316,	/*right center of big curly bracket*/
0312,	/*bold vertical*/
0321,	/*left floor (left bot of big sq bract)*/
0320,	/*right floor (rb of ")*/
0322,	/*left ceiling (lt of ")*/
0310};	/*right ceiling (rt of ")*/


/*	include default font character width tables	*/

#ifdef unix
#include "fonts.d/ftR.c"	/* Roman */
#include "fonts.d/ftI.c"	/* Italic */
#include "fonts.d/ftB.c"	/* Bold */
#include "fonts.d/ftS.c"	/* Special */
#endif

#ifdef ibm
#include "ftR.c"
#include "ftI.c"
#include "ftB.c"
#include "ftS.c"
#endif
