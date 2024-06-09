#define INCH 240
/*
HP 2631 Line Printer with col-only reverse line feed - in expanded mode
nroff driving tables
width and code tables
*/

struct termtable t2631e = {
/*bset*/	0,
/*breset*/	0,
/*Hor*/		INCH/5,
/*Vert*/	INCH/6,
/*Newline*/	INCH/6,
/*Char*/	INCH/5,
/*Em*/		INCH/5,
/*Halfline*/	INCH/12,
/*Adj*/		INCH/5,
/*twinit*/	"\033&k1S",
/*twrest*/	"\033E",
/*twnl*/	"\n",
/*hlr*/		"",
/*hlf*/		"",
/*flr*/		"\0337",
/*bdon*/	"",
/*bdoff*/	"",
/*iton*/	"",
/*itoff*/	"",
/*ploton*/	"",
/*plotoff*/	"",
/*up*/		"",
/*down*/	"",
/*right*/	"",
/*left*/	"",
/*codetab*/
#include "code.lp"
