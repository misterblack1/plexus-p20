%token	NAME CONST FLABEL DREG IF GOTO EQL NEQ EQ XOR
%token	INC DEC LSS LEQ GTR GEQ OR AND COMP NOT CO SC CM
%token	RP LP LC AM RC LB RB CONTINUE FUNCTION REPOP
%token	GET GETA XMT PUT CRC16 LRCGEN  RETURN
%token	RCV TIMEOUT RTNERR SNAP RTNXBUF RTNRBUF EXIT END
%token	PUTEA GETXBUF GETRBUF CRCLOC XSOM XEOM
%token	RSXBUF RSRBUF
%token	RSOM SUB ADD FNAME ARRAY TESTOP TRACE LSH RSH
%token	RCVFRM RSRFRM RTNRFRM XMTBUSY GETXFRM XMTFRM RTNXFRM
%token	SETCTL RSXMTQ XMTCTL ABTXFRM TIMER GETCMD RTNSTAT
%token	ATOE ETOA DSRWAIT NORBUF RTNRPT
%{
#define ZERO	1
#define OUNA	2
#define BINA	3
char fname[32],priex[32],bname[32],aname[32],offset[32];
char oline[80],ocode[256];
int stmtype,atype;
char *repop[] = {
	"MOVM",
	"ORM",
	"XORM",
	"ADDM",
	"SUBM",
	"ANDM",
	"LSHM",
	"RSHM"
	};
%}
%%
prog:		decl_list fn_list;
decl_list:	|
		decl_list decl;
decl:		ARRAY NAME act1 LB CONST RB SC =
		{
			sprintf(oline,"%s	BSS	%s\n",desreg,name);
			strcat(ocode,oline);
			prline();
		}|
		SC =
		{
			if($1 == 1)prline();
		};
fn_list:	fn|
		fn_list fn;
fn:		fn_dfn  stm_list END SC=
		{
			if(stmtype != 7){
				sprintf(oline,"	RETURNI	0\n");
				strcat(ocode,oline);
				prline();
			}
			stmtype = 0;
		};
fn_dfn:		FUNCTION fn_name SC =
		{
			sprintf(oline,"%s:\n",name);
			strcat(ocode,oline);
			fname[0] = '\0';
			strcpy(fname,name);
			prline();
		};
fn_name:	NAME LP RP;
stm_list:
		|
		stm_list stm;
stm:		expr SC =
		{
			switch($1){
			case CONST: case ZERO:
				sprintf(oline,"	MOVI	%s\n",name);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",name);
				strcat(ocode,oline);
				break;
			}
			stmtype = 1;
			prline();
		}|
		lvalue REPOP act1 expr SC =
		{
			if(debug == 1)
				printf("a  %d %d %d %d\n",$1,$2,$3,$4);
			stmtype = 2;
			switch($4){
			case ZERO:
				sprintf(oline,"	CLR	%s\n",desreg);
				strcat(ocode,oline);
				break;
			case CONST:
				sprintf(oline,"	MOVI	%s\n",name);
				strcat(ocode,oline);
				sprintf(oline,"	%s	%s\n",repop[$2],desreg);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",name);
				strcat(ocode,oline);
				sprintf(oline,"	%s	%s\n",repop[$2],desreg);
				strcat(ocode,oline);
				break;
			default:
				sprintf(oline,"	%s	%s\n",repop[$2],desreg);
				strcat(ocode,oline);
			}
			prline();
		}|
		IF LP expr RP GOTO CONST SC  = 
		{
			stmtype = 3;
			switch($3){
			case CONST: case ZERO:
				sprintf(oline,"	MOVI	%s\n",priex);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",priex);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	JMPNEZ	L%s\n",name);
			strcat(ocode,oline);
			prline();
		}|
		IF LP NOT expr RP GOTO CONST SC  = 
		{
			stmtype = 3;
			switch($4){
			case CONST: case ZERO:
				sprintf(oline,"	MOVI	%s\n",priex);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",priex);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	JMPEQZ	L%s\n",name);
			strcat(ocode,oline);
			prline();
		}|
		IF LP expr RP GOTO NAME SC  = 
		{
			stmtype = 3;
			switch($3){
			case CONST: case ZERO:
				sprintf(oline,"	MOVI	%s\n",priex);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",priex);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	JMPNEZ	%s\n",name);
			strcat(ocode,oline);
			prline();
		}|
		GOTO NAME SC  = 
		{
			stmtype = 4;
			sprintf(oline,"	JMP	%s\n",name);
			strcat(ocode,oline);
			prline();
		}|
		GOTO CONST SC  = 
		{
			stmtype = 4;
			sprintf(oline,"	JMP	L%s\n",name);
			strcat(ocode,oline);
			prline();
		}|
		CONTINUE SC	=
		{
			stmtype = 5;
			prline();
		}|
		RETURN  SC	=
		{
			stmtype = 7;
			sprintf(oline,"	RETURNI	0\n");
			strcat(ocode,oline);
			prline();
		}|
		FNAME REPOP  act1 expr SC RETURN SC =
		{
			switch($4){
			case CONST:
				sprintf(oline,"	RETURNI	%s\n",name);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	RETURN	%s\n",name);
				strcat(ocode,oline);
				break;
			default:
				sprintf(oline,"	%s	_%s\n",repop[$2],desreg);
				strcat(ocode,oline);
				sprintf(oline,"	RETURN	_%s\n",desreg);
				strcat(ocode,oline);
			}
			stmtype = 7;
			prline();
		}|
		SC =
		{
			stmtype = 6;
			if($1 == 1)prline();
		}|
		label stm =
		{
		}|
		error SC ;
expr:		una_expr|
		bin_expr =
		{
			$$ = BINA;
			if(debug == 1)
				printf("d  %d\n",$1);
		};
label:		NAME CO =
		{
			sprintf(oline,"%s:\n",name);
			strcat(ocode,oline);
		}|
		FLABEL =
		{
			sprintf(oline,"L%s:\n",name);
			strcat(ocode,oline);
		};
una_expr:	primary =
		{
			if(debug == 1)
				printf("b  %d\n",$1);
			$$ = $1;
			strcpy(priex,name);
		}|
		INC lvalue   = 
		{
			sprintf(oline,"	INC	%s\n",name);
			strcat(ocode,oline);
			$$ = OUNA;
		}|
		DEC lvalue   = 
		{
			sprintf(oline,"	DEC	%s\n",name);
			strcat(ocode,oline);
			$$ = OUNA;
		}|
		fn_name =
		{
			sprintf(oline,"	CALL	%s\n",name);
			strcat(ocode,oline);
			$$ = OUNA;
		}|
		sys_call =
		{
			$$ = OUNA;
		}|
		NOT una_expr =
		{
			switch($2){
			case ZERO: case CONST:
				sprintf(oline,"	MOVI	%s\n",name);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",name);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	TSTEQLI	0\n");
			strcat(ocode,oline);
			$$ = OUNA;
		}|
		COMP una_expr =
		{
			switch($2){
			case ZERO: case CONST:
				sprintf(oline,"	MOVI	%s\n",name);
				strcat(ocode,oline);
				break;
			case NAME:
				sprintf(oline,"	MOV	%s\n",name);
				strcat(ocode,oline);
				break;
			}
			sprintf(oline,"	XORI	0377\n");
			strcat(ocode,oline);
			$$ = OUNA;
		}|
		LP expr RP =
		{
			$$ = $2;
		};
bin_expr:	una_expr ADD primary =
		{
			bopcode("ADD",$1,$3);
		}|
		una_expr SUB primary =
		{
			bopcode("SUB",$1,$3);
		}|
		una_expr OR primary =
		{
			bopcode("OR",$1,$3);
		}|
		una_expr XOR primary =
		{
			bopcode("XOR",$1,$3);
		}|
		una_expr AND primary =
		{
			bopcode("AND",$1,$3);
		}|
		una_expr LSH primary =
		{
			bopcode("LSH",$1,$3);
		}|
		una_expr RSH primary =
		{
			bopcode("RSH",$1,$3);
		}|
		una_expr AND COMP primary =
		{
			bopcode("BIC",$1,$4);
		}|
		una_expr EQL primary =
		{
			bopcode("TSTEQL",$1,$3);
		}|
		una_expr NEQ primary =
		{
			bopcode("TSTNEQ",$1,$3);
		}|
		una_expr LSS primary =
		{
			bopcode("TSTLSS",$1,$3);
		}|
		una_expr LEQ primary =
		{
			bopcode("TSTLEQ",$1,$3);
		}|
		una_expr GTR primary =
		{
			bopcode("TSTGTR",$1,$3);
		}|
		una_expr GEQ primary =
		{
			bopcode("TSTGEQ",$1,$3);
		};
sys_call:	GET LP lvalue RP =
		{
			sccode(1,$3,0,"GETBYTE");
		}|
		XMT LP primary RP =
		{
			sccode(1,$3,0,"XMT");
		}|
		RCV LP lvalue RP =
		{
			sccode(1,$3,0,"RCV");
		}|
		XSOM LP CONST RP =
		{
			sccode(1,$3,0,"XSOM");
		}|
		RSOM LP CONST RP =
		{
			sccode(1,$3,0,"RSOM");
		}|
		XEOM LP CONST RP =
		{
			sccode(1,$3,0,"XEOM");
		}|
		PUT LP primary RP =
		{
			sccode(1,$3,0,"PUTBYTE");
		}|
		CRC16 LP primary RP =
		{
			sccode(1,$3,0,"CRC16");
		}|
		TESTOP LP primary RP =
		{
			sccode(1,$3,0,"TESTOP");
		}|
		ATOE LP primary RP =
		{
			sccode(1,$3,0,"ATOE");
		}|
		ETOA LP primary RP =
		{
			sccode(1,$3,0,"ETOA");
		}|
		CRCLOC LP NAME RP =
		{
			sccode(1,$3,0,"CRCLOC");
		}|
		DSRWAIT LP  RP =
		{
			sccode(0,0,0,"DSRWAIT");
		}|
		TIMEOUT LP primary RP =
		{
			sccode(1,$3,0,"TIMEOUT");
		}|
		RTNERR LP primary RP =
		{
			sccode(1,$3,0,"RTNERR");
		}|
		TRACE LP primary RP =
		{
			sccode(1,$3,0,"TRACE1");
		}|
		TRACE LP primary act2 CM primary RP =
		{
			sccode(2,$3,$6,"TRACE2");
		}|
		SNAP LP NAME RP =
		{
			sccode(1,$3,0,"SNAP");
		}|
		RTNXBUF LP NAME RP =
		{
			sccode(1,$3,0,"RTNXBUF");
		}|
		RTNRBUF LP NAME RP =
		{
			sccode(1,$3,0,"RTNRBUF");
		}|
		GETXBUF LP NAME RP =
		{
			sccode(1,$3,0,"GETXBUF");
		}|
		GETRBUF LP NAME RP =
		{
			sccode(1,$3,0,"GETRBUF");
		}|
		RSRBUF LP RP =
		{
			sccode(0,0,0,"RSRBUF");
		}|
		RSXBUF LP RP =
		{
			sccode(0,0,0,"RSXBUF");
		}|
		EXIT LP primary RP =
		{
			sccode(1,$3,0,"HALT");
		}|
		RCVFRM LP NAME RP =
		{
			sccode(1,$3,0,"RCVFRM");
		}|
		SETCTL LP NAME act2 CM primary RP =
		{
			sccode(2,$3,$6,"SETCTL");
		}|
		GETXFRM LP lvalue RP =
		{
			sccode(1,$3,0,"GETXFRM");
		}|
		XMTFRM LP primary RP =
		{
			sccode(1,$3,0,"XMTFRM");
		}|
		XMTCTL LP RP =
		{
			sccode(0,$3,0,"XMTCTL");
		}|
		RTNXFRM LP lvalue RP =
		{
			sccode(1,$3,0,"RTNXFRM");
		}|
		RSRFRM LP  RP =
		{
			sccode(0,0,0,"RSRFRM");
		}|
		RTNRFRM LP  RP =
		{
			sccode(0,0,0,"RTNRFRM");
		}|
		XMTBUSY LP  RP =
		{
			sccode(0,0,0,"XMTBUSY");
		}|
		NORBUF LP  RP =
		{
			sccode(0,0,0,"NORBUF");
		}|
		RSXMTQ LP  RP =
		{
			sccode(0,0,0,"RSXMTQ");
		}|
		ABTXFRM LP  RP =
		{
			sccode(0,0,0,"ABTXFRM");
		}|
		GETCMD LP NAME RP =
		{
			sccode(1,$3,0,"GETCMD");
		}|
		RTNRPT LP NAME RP =
		{
			sccode(1,$3,0,"RTNRPT");
		}|
		TIMER LP CONST RP =
		{
			sccode(1,$3,0,"TIMER");
		}|
		RTNSTAT LP NAME RP =
		{
			sccode(1,$3,0,"RTNSTAT");
		};
primary:	lvalue|
		CONST =
		{
			if(strcmp(name,"0") == 0)
				$$ = ZERO;
			else
				$$ = CONST;
		}|
		LP primary RP =
		{
			$$ = $2;
		};
lvalue:		NAME act3 LB CONST RB =
		{
			strcpy(&offset[0],name);
			strcpy(&name[0],bname);
			strcat(name,"+");
			strcat(name,offset);
			$$ = NAME;
		}|
		NAME =
		{
			$$ = NAME;
			if(debug == 1)
					printf("e  %d\n",$1);
		};
act1:		={
			strcpy(desreg,name);
		};
act2:		={
			strcpy(&aname[0],name);
			atype = NAME;
		};
act3:
		={
			strcpy(&bname[0],name);
			atype = NAME;
		};
%%
int debug = 0;
int yyline = 1;
#include "pl.yl"
char	desreg[32],srcreg[32];
main(argc,argv)
int argc;
char *argv[];
{
	yyinit(argc,argv);
	if(yyparse())
		return;
	yyaccpt();
}
yyinit(argc,argv)
int argc;
char *argv[];
{
	while(argc > 1 && argv[1][0] == '-'){
		switch(argv[1][1]){
			case 'd':
				debug = atoi(&argv[1][2]);
				if(debug == 1)
					printf("debug = %d\n",debug);
				break;
		}
		argc--;
		argv++;
	}
}
yyaccpt()
{
}
yyerror(s)
char *s;
{
	extern int yyline;
	fprintf(stderr,"\n***** pl: %s",s);
	if(yyline)
		fprintf(stderr,",input line %d.\n",yyline);
}
bopcode(s,lside,rside)
char *s;
int lside,rside;
{
	switch(lside){
		case CONST: case ZERO:
			sprintf(oline,"	MOVI	%s\n",priex);
			strcat(ocode,oline);
			break;
		case NAME:
			sprintf(oline,"	MOV	%s\n",priex);
			strcat(ocode,oline);
	}
	if(rside == NAME){
		sprintf(oline,"	%s	%s\n",s,name);
		strcat(ocode,oline);
	}else{
		sprintf(oline,"	%sI	%s\n",s,name);
		strcat(ocode,oline);
	}
}
sccode(argct,arg1,arg2,priname)
char *priname;
{
	switch(argct){
	case 0:
		sprintf(oline,"	%s\n",priname);
		strcat(ocode,oline);
		break;
	case 1:
		if(arg1 == NAME){
			sprintf(oline,"	%s	%s\n",priname,name);
			strcat(ocode,oline);
		}else{
			sprintf(oline,"	%sI	%s\n",priname,name);
			strcat(ocode,oline);
		}
		break;
	case 2:
		if(arg2 == NAME){
			sprintf(oline,"	MOV	%s\n",name);
			strcat(ocode,oline);
		}else{
			sprintf(oline,"	MOVI	%s\n",name);
			strcat(ocode,oline);
		}
		if(arg1 == atype){
			sprintf(oline,"	%s	%s\n",priname,aname);
			strcat(ocode,oline);
		}else{
			sprintf(oline,"	%sI	%s\n",priname,aname);
			strcat(ocode,oline);
		}
	}
}
prline()
{
	if(debug == 2){
		printf("*%d	%s\n",yyline,sline);
		sline[0]='\0';
	}
	printf("%s",ocode);
	ocode[0]='\0';
}
