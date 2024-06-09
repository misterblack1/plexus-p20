#define INCH 240
/*
TX TRAIN PRINTER
nroff driving tables
width and code tables
*/

struct termtable tX =	{
/*bset*/	0,
/*breset*/	0,
/*Hor*/		INCH/60,
/*Vert*/	INCH/48,
/*Newline*/	INCH/6,
/*Char*/	INCH/10,
/*Em*/		INCH/10,
/*Halfline*/	INCH/12,
/*Adj*/		INCH/10,
/*twinit*/	"",
/*twrest*/	"",
/*twnl*/	"\r\n",
/*hlr*/		"\033D",
/*hlf*/		"\033U",
/*flr*/		"\033\n",
/*bdon*/	"\033B",
/*bdoff*/	"\033R",
/*iton*/	"\033I",
/*itoff*/	"\033R",
/*ploton*/	"",
/*plotoff*/	"",
/*up*/		"\033\n",
/*down*/	"\n",
/*right*/	" ",
/*left*/	"\b",
/*codetab*/
"\001 ",	/*space*/
"\001!",	/*!*/
"\001\"",	/*"*/
"\001#",	/*#*/
"\001$",	/*$*/
"\001%",	/*%*/
"\001&",	/*&*/
"\001'",	/*' close*/
"\001(",	/*(*/
"\001)",	/*)*/
"\001*",	/***/
"\001+",	/*+*/
"\001,",	/*,*/
"\001-",	/*- hyphen*/
"\001.",	/*.*/
"\001/",	/*/*/
"\2010",	/*0*/
"\2011",	/*1*/
"\2012",	/*2*/
"\2013",	/*3*/
"\2014",	/*4*/
"\2015",	/*5*/
"\2016",	/*6*/
"\2017",	/*7*/
"\2018",	/*8*/
"\2019",	/*9*/
"\001:",	/*:*/
"\001;",	/*;*/
"\001<",	/*<*/
"\001=",	/*=*/
"\001>",	/*>*/
"\001?",	/*?*/
"\001@",	/*@*/
"\201A",	/*A*/
"\201B",	/*B*/
"\201C",	/*C*/
"\201D",	/*D*/
"\201E",	/*E*/
"\201F",	/*F*/
"\201G",	/*G*/
"\201H",	/*H*/
"\201I",	/*I*/
"\201J",	/*J*/
"\201K",	/*K*/
"\201L",	/*L*/
"\201M",	/*M*/
"\201N",	/*N*/
"\201O",	/*O*/
"\201P",	/*P*/
"\201Q",	/*Q*/
"\201R",	/*R*/
"\201S",	/*S*/
"\201T",	/*T*/
"\201U",	/*U*/
"\201V",	/*V*/
"\201W",	/*W*/
"\201X",	/*X*/
"\201Y",	/*Y*/
"\201Z",	/*Z*/
"\001[",	/*[*/
"\001\\",	/*\*/
"\001]",	/*]*/
"\001^",	/*^*/
"\001_",	/*_ dash*/
"\001`",	/*` open*/
"\201a",	/*a*/
"\201b",	/*b*/
"\201c",	/*c*/
"\201d",	/*d*/
"\201e",	/*e*/
"\201f",	/*f*/
"\201g",	/*g*/
"\201h",	/*h*/
"\201i",	/*i*/
"\201j",	/*j*/
"\201k",	/*k*/
"\201l",	/*l*/
"\201m",	/*m*/
"\201n",	/*n*/
"\201o",	/*o*/
"\201p",	/*p*/
"\201q",	/*q*/
"\201r",	/*r*/
"\201s",	/*s*/
"\201t",	/*t*/
"\201u",	/*u*/
"\201v",	/*v*/
"\201w",	/*w*/
"\201x",	/*x*/
"\201y",	/*y*/
"\201z",	/*z*/
"\001{",	/*{*/
"\001|",	/*|*/
"\001}",	/*}*/
"\001~",	/*~*/
"\000\0",	/*narrow sp*/
"\001-",	/*hyphen*/
"\001\033X\127",	/* bullet */
"\001\033X\115",	/*square*/
"\001\033X\147",	/*3/4 em*/
"\001\033X\147",	/*rule*/
"\0031/4",	/*1/4*/
"\0031/2",	/*1/2*/
"\0033/4",	/*3/4*/
"\001-",	/*minus*/
"\202fi",	/*fi*/
"\202fl",	/*fl*/
"\202ff",	/*ff*/
"\203ffi",	/*ffi*/
"\203ffl",	/*ffl*/
"\001\033X\130",	/*degree*/
"\001|\b\033X\121",	/*dagger*/
"\001S\b|",	/*section*/
"\001\033X\112",	/*foot mark*/
"\001'",	/*acute accent*/
"\001`",	/*grave accent*/
"\001_",	/*underrule*/
"\001/",	/*slash (longer)*/
"\000\0",	/*half narrow space*/
"\001 ",	/*unpaddable space*/
"\001a",	/*alpha*/
"\001B\b,",	/*beta*/
"\001)\b/",	/*gamma*/
"\001o\b`",	/*delta*/
"\001<\b-",	/*epsilon*/
"\001/\b`\b_",	/*zeta*/
"\001n\b`",	/*eta*/
"\001O\b-",	/*theta*/
"\001i",	/*iota*/
"\001k",	/*kappa*/
"\001)\b\\",	/*lambda*/
"\001u\b,",	/*mu*/
"\001v",	/*nu*/
"\001(\b-\b`",	/*xi*/
"\001o",	/*omicron*/
"\001n\b\033X\147",	/*pi*/
"\001o\b,",	/*rho*/
"\001o\b'",	/*sigma*/
"\001t",	/*tau*/
"\001v",	/*upsilon*/
"\001o\b/",	/*phi*/
"\001x",	/*chi*/
"\001u\b/",	/*psi*/
"\001w",	/*omega*/
"\001I\b\033X\121",	/*Gamma*/
"\002/\b__\b\\",	/*Delta*/
"\001O\b=",	/*Theta*/
"\002/\\",	/*Lambda*/
"\000\0",	/*Xi*/
"\002[\b][\b]",	/*Pi*/
"\001>\b_\b\033\n_\n",	/*Sigma*/
"\000\0",	/**/
"\001Y",	/*Upsilon*/
"\001o\b[\b]",	/*Phi*/
"\001U\b|",	/*Psi*/
"\002_\b()\b_",	/*Omega*/
"\001/",	/*square root*/
"\000\0",	/*terminal sigma*/
"\001\033\n_\n",	/*root en*/
"\001\033X\126",	/*>=*/
"\001\033X\106",	/*<=*/
"\001=\b_",	/*identically equal*/
"\001\033X\147",	/*equation minus*/
"\001=\b~",	/*approx =*/
"\001~",	/*approximates*/
"\001\033X\146",	/*not equal*/
"\002->",	/*right arrow*/
"\002<-",	/*left arrow*/
"\001|\b^",	/*up arrow*/
"\001|\bv",	/*down arrow*/
"\001=",	/*equation equal*/
"\001/\b\\",	/*multiply*/
"\001:\b-",	/*divide*/
"\001\033X\117",	/*plus-minus*/
"\001U",	/*cup (union)*/
"\000\0",	/*cap (intersection)*/
"\000\0",	/*subset of*/
"\000\0",	/*superset of*/
"\000\0",	/*improper subset*/
"\000\0",	/*improper superset*/
"\002oo",	/*infinity*/
"\001o\b)",	/*partial derivative*/
"\002\\/",	/*gradient*/
"\001\033X\145",	/*not*/
"\001|\b,\b'",	/*integral sign*/
"\000\0",	/*proportional to*/
"\000O\b/",	/*empty set*/
"\001(\b-",	/*member of*/
"\001\033X\111",	/*equation plus*/
"\003(r)",	/*registered*/
"\003(c)",	/*copyright*/
"\001|",	/*box rule */
"\001\033X\125",	/*cent sign*/
"\001|\b=",	/*dbl dagger*/
"\002->",	/*right hand*/
"\002<-",	/*left hand*/
"\001*",	/*math * */
"\000\033L",	/*bell system sign*/
"\001|",	/*or (was star)*/
"\001O",	/*circle*/
"\001\033X\124",	/*left top (of big curly)*/
"\001\033X\123",	/*left bottom*/
"\001\033X\144",	/*right top*/
"\001\033X\143",	/*right bot*/
"\001\033X\143\b\033X\144",	/*left center of big curly bracket*/
"\001\033X\123\b\033X\124",	/*right center of big curly bracket*/
"\001|",	/*bold vertical*/
"\001\033X\123",	/*left floor (left bot of big sq bract)*/
"\001\033X\143",	/*right floor (rb of ")*/
"\001\033X\124",	/*left ceiling (lt of ")*/
"\001\033X\144"	/*right ceiling (rt of ")*/
};
