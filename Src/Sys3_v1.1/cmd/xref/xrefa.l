%k 100
%a 1000
%o 1000
%n 300
%e 500
%p 3000
%{
char	*filename="-";
int	pflag=0;

main(argc,argv)
int	argc;
char	*argv[];
{
	register int rc=0;
	if(argc<=1) {
		yylex();
	}else{
		while(argc>1) {
			if(strcmp(argv[1],"-p")==0) {
				pflag++;
			}else if(freopen(argv[1],"r",stdin)==NULL) {
				fprintf(stderr,"xref: %s: cannot open\n", argv[1]);
				rc++;
			}else{
				filename=argv[1];
				yylineno=1;
				yylex();
			}
			argc--; argv++;
		}
	}
	return(rc);
}
fndef()
{
	/* fn defs */
	register char *p = &yytext[yyleng];
	while(*--p != ')'){
		ungetc(*p,stdin);
		if(*p == '\n') yylineno--;
	}
	*++p = 0;
	while(*--p!='('){
		if (*p == '\n') yylineno--;
		ungetc(*p,stdin);
		*p = 0;
	}
	while(*p==' ' || *p=='\t') {
		p--;
		if(p<yytext) break;
	}
	while(*p!=' ' && *p!='\t') {
		p--;
		if(p<yytext) break;
	}
	p++;
	printf("%s)", p);
	if(p!=yytext) {
		*--p=0;
		printf("/%s", yytext);
	}
	printf(":%s:+%d\n", filename, yylineno);
}
%}
%%
"/*"				{
				/* comments */
				char c;
				while(c=yyinput())
					if(c=='*' && yyinput()=='/')
						break;
				}
"\""				{
				/* strings */
				char c;
				while(c=yyinput())
					if(c=='"')
						break;
					else if(c=='\\') yyinput();
				}
^[a-zA-Z_][a-zA-Z0-9 \t_]*"("[a-zA-Z0-9, ]*")"([\n\t ]*[a-zA-Z0-9,* \t_]*";")*[\n\t ]*"{"	{
					fndef();
				}
if[ \t]*"("	;
return[ \t]*"("	;
while[ \t]*"("	;
for[ \t]*"("	;
switch[ \t]*"("	;
break		;
extern		;
short		;
ifdef		;
endif		;
long		;
return		;
register	;
include		;
define		;
continue	;
int		;
char		;
float		;
static		;
auto		;
unsigned	;
double		;
else		;
do		;
default		;
struct		;
union		;
[a-zA-Z_][a-zA-Z0-9_]*[ \t]*"("	{
				/* fn calls */
				printf("%s):%s:%d\n", yytext, filename, yylineno);
				}
[a-zA-Z_][a-zA-Z0-9_]*		{
				/* id references */
				if(pflag==0)
					printf("%s:%s:%d\n", yytext, filename, yylineno);
				}
.	;
\n	;
