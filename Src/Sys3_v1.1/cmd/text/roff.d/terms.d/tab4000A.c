#define INCH 240
/*
Trendata 4000A
nroff driving tables
width and code tables
*/

struct termtable t4000A = {
/*bset*/	0,
/*breset*/	054,
/*Hor*/		INCH/60,
/*Vert*/	INCH/48,
/*Newline*/	INCH/6,
/*Char*/	INCH/10,
/*Em*/		INCH/10,
/*Halfline*/	INCH/12,
/*Adj*/		INCH/10,
/*twinit*/	"\033H0",
/*twrest*/	"\033H0",
/*twnl*/	"\015\n",
/*hlr*/		"\033H4\033-Y0004\033H0",
/*hlf*/		"\033H4\033+Y0004\033H0",
/*flr*/		"\0337\0337\0337\0337\033\033",
/*bdon*/	"",
/*bdoff*/	"",
/*iton*/	"",
/*itoff*/	"",
/*ploton*/	"\033H4",
/*plotoff*/	"\033H0",
/*up*/		"\0337",
/*down*/	"\n",
/*right*/	" ",
/*left*/	"\b",
/*codetab*/
#include "code.300"
