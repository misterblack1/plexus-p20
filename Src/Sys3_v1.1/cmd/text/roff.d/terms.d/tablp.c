#define INCH 240
/*
Line Printer with col-only reverse line feed
nroff driving tables
width and code tables
*/

struct termtable tlp = {
/*bset*/	0,
/*breset*/	0,
/*Hor*/		INCH/10,
/*Vert*/	INCH/6,
/*Newline*/	INCH/6,
/*Char*/	INCH/10,
/*Em*/		INCH/10,
/*Halfline*/	INCH/12,
/*Adj*/		INCH/10,
/*twinit*/	"",
/*twrest*/	"",
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
