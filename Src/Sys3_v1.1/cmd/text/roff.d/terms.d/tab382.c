#define INCH 240
/*
DTC382
nroff driving tables
width and code tables
*/

struct termtable t382 = {
/*bset*/	0,
/*breset*/	054,
/*Hor*/		INCH/60,
/*Vert*/	INCH/48,
/*Newline*/	INCH/6,
/*Char*/	INCH/10,
/*Em*/		INCH/10,
/*Halfline*/	INCH/12,
/*Adj*/		INCH/10,
/*twinit*/	"\033\006",
/*twrest*/	"\033A",
/*twnl*/	"\015\012",
/*hlr*/		"\033H",
/*hlf*/		"\033h",
/*flr*/		"\032",
/*bdon*/	"\033Z",
/*bdoff*/	"\033z",
/*iton*/	"",
/*itoff*/	"",
/*ploton*/	"\006",
/*plotoff*/	"\033\006",
/*up*/		"\032",
/*down*/	"\012",
/*right*/	" ",
/*left*/	"\010",
/*codetab*/
#include "code.300"
