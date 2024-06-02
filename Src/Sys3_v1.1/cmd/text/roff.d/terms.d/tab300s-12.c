#define INCH 240
/*
DASI300S
nroff driving tables
width and code tables
*/

struct termtable t300s12 = {
/*bset*/	0,
/*breset*/	054,
/*Hor*/		INCH/60,
/*Vert*/	INCH/48,
/*Newline*/	INCH/8,
/*Char*/	INCH/12,
/*Em*/		INCH/12,
/*Halfline*/	INCH/16,
/*Adj*/		INCH/12,
/*twinit*/	"\033\006",
/*twrest*/	"\033\006",
/*twnl*/	"\015\n",
/*hlr*/		"",
/*hlf*/		"",
/*flr*/		"\032",
/*bdon*/	"\033E",
/*bdoff*/	"\033E",
/*iton*/	"",
/*itoff*/	"",
/*ploton*/	"\006",
/*plotoff*/	"\033\006",
/*up*/		"\032",
/*down*/	"\n",
/*right*/	" ",
/*left*/	"\b",
/*codetab*/
#include "code.300"
