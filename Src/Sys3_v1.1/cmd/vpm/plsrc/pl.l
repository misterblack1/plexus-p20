%{
#define echo	if(debug == 2)strcat(sline,yytext);
extern yylval;
extern int yyline;
char sline[80];
char *infile,instr[32];
char name[32],dreg[32],intr[32];
%}
D	[0-9]
H	[0-9a-fA-F]
L	[a-zA-Z]
AN	[0-9a-zA-Z]
A	[\000-\0177]
E	[\000-\0377]
%%
array		{echo; return(ARRAY);}
if		{echo; return(IF);}
goto		{echo; return(GOTO);}
continue	{echo; return(CONTINUE);}
function	{echo; return(FUNCTION);}
end		{echo; return(END);}
get		{echo; return(GET);}
xmt		{echo; return(XMT);}
xsom		{echo; return(XSOM);}
rsom		{echo; return(RSOM);}
xeom		{echo; return(XEOM);}
rcv		{echo; return(RCV);}
put		{echo; return(PUT);}
putea		{echo; return(PUTEA);}
crc16		{echo; return(CRC16);}
rsrbuf		{echo; return(RSRBUF);}
rsxbuf		{echo; return(RSXBUF);}
testop		{echo; return(TESTOP);}
atoe		{echo; return(ATOE);}
etoa		{echo; return(ETOA);}
crcloc		{echo; return(CRCLOC);}
dsrwait		{echo; return(DSRWAIT);}
lrcgen		{echo; return(LRCGEN);}
return		{echo; return(RETURN);}
timeout		{echo; return(TIMEOUT);}
rtnerr		{echo; return(RTNERR);}
trace		{echo; return(TRACE);}
snap		{echo; return(SNAP);}
rtnxbuf		{echo; return(RTNXBUF);}
rtnrbuf		{echo; return(RTNRBUF);}
getxbuf		{echo; return(GETXBUF);}
getrbuf		{echo; return(GETRBUF);}
exit		{echo; return(EXIT);}
rcvfrm		{echo; return(RCVFRM);}
rtnrfrm		{echo; return(RTNRFRM);}
xmtbusy		{echo; return(XMTBUSY);}
norbuf		{echo; return(NORBUF);}
getxfrm		{echo; return(GETXFRM);}
xmtfrm		{echo; return(XMTFRM);}
rtnxfrm		{echo; return(RTNXFRM);}
setctl		{echo; return(SETCTL);}
rsxmtq		{echo; return(RSXMTQ);}
rsrfrm		{echo; return(RSRFRM);}
xmtctl		{echo; return(XMTCTL);}
abtxfrm		{echo; return(ABTXFRM);}
getcmd		{echo; return(GETCMD);}
rtnrpt		{echo; return(RTNRPT);}
timer		{echo; return(TIMER);}
rtnstat		{echo; return(RTNSTAT);}
"=="		{echo; return(EQL);}
".eq."		{echo; return(EQL);}
"!="		{echo; return(NEQ);}
".ne."		{echo; return(NEQ);}
"<"		{echo; return(LSS);}
".lt."		{echo; return(LSS);}
"<="		{echo; return(LEQ);}
".le."		{echo; return(LEQ);}
">"		{echo; return(GTR);}
".gt."		{echo; return(GTR);}
">="		{echo; return(GEQ);}
".ge."		{echo; return(GEQ);}
"-"		{echo; return(SUB);}
"+"		{echo; return(ADD);}
"^"		{echo; return(XOR);}
"`"		{echo; return(XOR);}
"`"		{echo; return(XOR);}
"|"		{echo; return(OR);}
".or."		{echo; return(OR);}
"&"		{echo; return(AND);}
".and."		{echo; return(AND);}
"<<"		{echo; return(LSH);}
">>"		{echo; return(RSH);}
"="		{echo; yylval = 0;  return(REPOP);}
"|="		{echo; yylval = 1;  return(REPOP);}
".or.="		{echo; yylval = 1;  return(REPOP);}
"^="		{echo; yylval = 2;  return(REPOP);}
"`="		{echo; yylval = 2;  return(REPOP);}
"+="		{echo; yylval = 3;  return(REPOP);}
"-="		{echo; yylval = 4;  return(REPOP);}
"&="		{echo; yylval = 5;  return(REPOP);}
".and.="	{echo; yylval = 5;  return(REPOP);}
"<<="		{echo; yylval = 6;  return(REPOP);}
">>="		{echo; yylval = 7;  return(REPOP);}
"--"		{echo; return(DEC);}
"++"		{echo; return(INC);}
"~"		{echo; return(COMP);}
"!"		{echo; return(NOT);}
".not."		{echo; return(NOT);}
":"		{echo; return(CO);}
";"		{echo; yylval = 1;return(SC);}
\n		{
			yyline++;
			yylval = 2;
			return(SC);
		}
","		{echo; return(CM);}
")"		{echo; return(RP);}
"("		{echo; return(LP);}
"{"		{echo; return(LC);}
"}"		{echo; return(RC);}
"["		{echo; return(LB);}
"]"		{echo; return(RB);}
^#.*\n		{
		if(sscanf(&yytext[1],"%d%*[\" ]%[^\"\n]",&yyline,infile = instr)<1)
			yyerror("illegal preprocessor line");
		}
"/*"		{
			for(;;){
			char c;
				while(input() != '*');
					if((c = input()) == '/')
						break;
				unput(c);
			}
		}
^{D}+		{
			yytext[31] ='\0';
			strcpy(name,yytext);
			yylval = NAME;
			echo;
			return(FLABEL);
		}
{D}+		|
0[0-7]+		|
0x{H}{H}	|
'.'		|
'\\.'		{
			echo;
			yytext[31] = '\0';
			strcpy(name,yytext);
			yylval = CONST;
			return(CONST);
		}
{L}{AN}*	{
			echo;
			yytext[31] = '\0';
			strcpy(name,yytext);
			yylval = NAME;
			if(strcmp(name,fname) == 0)
				return(FNAME);
			else
				return(NAME);
		}
.		echo;
