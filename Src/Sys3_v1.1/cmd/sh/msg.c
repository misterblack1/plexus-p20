#
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */


#include	"defs.h"
#include	"sym.h"

MSG		version = "\nVERSION sys137	DATE 1978 Oct 12 22:39:57\n";

/* error messages */
MSG	badopt		= "bad option(s)";
MSG	mailmsg		= "you have mail\n";
MSG	nospace		= "no space";
MSG	synmsg		= "syntax error";

MSG	badnum		= "bad number";
MSG	badparam	= "parameter null or not set";
MSG	unset		= "parameter not set";
MSG	badsub		= "bad substitution";
MSG	badcreate	= "cannot create";
MSG	illegal		= "illegal io";
MSG	restricted	= "restricted";
MSG	nofork		= "cannot fork: too many processes";
MSG	noswap		= "cannot fork: no swap space";
MSG	piperr		= "cannot make pipe";
MSG	badopen		= "cannot open";
MSG	coredump	= " - core dumped";
MSG	arglist		= "arg list too long";
MSG	txtbsy		= "text busy";
MSG	toobig		= "too big";
MSG	badexec		= "cannot execute";
MSG	notfound	= "not found";
MSG	badfile		= "bad file number";
MSG	badshift	= "cannot shift";
MSG	baddir		= "bad directory";
MSG	badtrap		= "bad trap";
MSG	wtfailed	= "is read only";
MSG	notid		= "is not an identifier";

/*	messages for 'builtin' functions	*/

MSG	btest		= "test";
MSG	badop	= "unknown operator ";

/* built in names */
MSG	pathname	= "PATH";
MSG	homename	= "HOME";
MSG	mailname	= "MAIL";
MSG	fngname		= "FILEMATCH";
MSG	ifsname		= "IFS";
MSG	ps1name		= "PS1";
MSG	ps2name		= "PS2";

/* string constants */
MSG	nullstr		= "";
MSG	sptbnl		= " \t\n";
MSG	defpath		= ":/bin:/usr/bin";
MSG	colon		= ": ";
MSG	minus		= "-";
MSG	endoffile	= "end of file";
MSG	unexpected 	= " unexpected";
MSG	atline		= " at line ";
MSG	devnull		= "/dev/null";
MSG	execpmsg	= "+ ";
MSG	readmsg		= "> ";
MSG	stdprompt	= "$ ";
MSG	supprompt	= "# ";
MSG	profile		= ".profile";
MSG	sysprofile	= "/etc/profile";


/* tables */
SYSTAB reserved={
		{"in",		INSYM},
		{"esac",	ESSYM},
		{"case",	CASYM},
		{"for",		FORSYM},
		{"done",	ODSYM},
		{"if",		IFSYM},
		{"while",	WHSYM},
		{"do",		DOSYM},
		{"then",	THSYM},
		{"else",	ELSYM},
		{"elif",	EFSYM},
		{"fi",		FISYM},
		{"until",	UNSYM},
		{ "{",		BRSYM},
		{ "}",		KTSYM},
		{0,	0},
};

STRING	sysmsg[]={
		0,
		"Hangup",
		0,	/* Interrupt */
		"Quit",
		"Illegal instruction",
		"Trace/BPT trap",
		"abort",
		"EMT trap",
		"Floating exception",
		"Killed",
		"Bus error",
		"Memory fault",
		"Bad system call",
		0,	/* Broken pipe */
		"Alarm call",
		"Terminated",
		"Signal 16",
};

MSG		export = "export";
MSG		duperr = "cannot dup";
MSG		readonly = "readonly";
SYSTAB	commands={
		{"cd",		SYSCD},
		{"read",	SYSREAD},
		{"set",		SYSSET},
		{":",		SYSNULL},
		{"trap",	SYSTRAP},
		{"wait",	SYSWAIT},
		{"eval",	SYSEVAL},
		{".",		SYSDOT},
		{readonly,	SYSRDONLY},
		{export,	SYSXPORT},
		{"break",	SYSBREAK},
		{"continue",	SYSCONT},
		{"shift",	SYSSHFT},
		{"exit",	SYSEXIT},
		{"exec",	SYSEXEC},
		{"times",	SYSTIMES},
#ifndef RES	/*	exclude umask and newgrp code from res vers.	*/
		{"umask",	SYSUMASK},
		{"newgrp", 	SYSNEWGRP},
#else
		{"login",	SYSLOGIN},
		{"newgrp",	SYSLOGIN},
#endif
		{0,	0},
};
SYSTAB xcmds={
		{"test",	TEST},
#ifndef RES
		{"[",		TEST},
#endif
		{0,		0},
};
