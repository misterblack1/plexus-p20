#include <stdio.h>
#include <ctype.h>
#include <pwd.h>

#define ERROR1 "Too many/few fields"
#define ERROR2a "No group name"
#define ERROR2b "Bad character(s) in group name"
#define ERROR3  "Invalid GID"
#define ERROR4a "Null login name"
#define ERROR4b "Logname not found in password file"

int eflag, badchar, baddigit,badlognam,colons,len,i;
char buf[512];
char tmpbuf[512];

struct passwd *getpwnam();
char *strchr();
char *nptr;
int setpwent();
char *cptr;
FILE *fptr;
int delim[512];
long gid;
int error();

main (argc,argv)
int argc;
char *argv[];
{
  if ( argc == 1)
    argv[1] = "/etc/group";
  else if ( argc != 2 )
       {
	 printf ("\nusage: %s filename\n\n",*argv);
	 exit(1);
       }

  if ( ( fptr = fopen (argv[1],"r" ) ) == NULL )
  { 
	printf ("\ncannot open file %s\n\n",argv[1]);
	exit(1);
  }

  while(fgets(buf,512,fptr) != NULL )
  {
	if ( buf[0] == '\n' )    /* blank lines are ignored */
          continue;

	for (i=0; buf[i]!=NULL; i++)
	{
	  tmpbuf[i]=buf[i];          /* tmpbuf is a work area */
	  if (tmpbuf[i] == '\n')     /* newline changed to comma */  
	    tmpbuf[i] = ',';
	}

	for (i; i <= 512; ++i)     /* blanks out rest of tmpbuf */ 
	{
	  tmpbuf[i] = NULL;
	}
	colons=0;
	eflag=0;
	badchar=0;
	baddigit=0;
	badlognam=0;
	gid=0l;

    /*	Check number of fields	*/

	for (i=0 ; buf[i]!=NULL ; i++)
	{
	  if (buf[i]==':')
          {
            delim[colons]=i;
            ++colons;
          }
	}
	if (colons != 3 )
	{
	  error(ERROR1);
	  continue;
	}

    /*	check to see that group name is at least 1 character	*/
    /*		and that all characters are printable.		*/
 
	if ( buf[0] == ':' )
	  error(ERROR2a);
	else
	{
	  for ( i=0; buf[i] != ':'; i++ )
	  {
	    if ( ! ( isprint(buf[i])))
		badchar++;
	  }
	  if ( badchar > 0 )
	    error(ERROR2b);
	}

    /*	check that GID is numeric and <= 65535	*/

	len = ( delim[2] - delim[1] ) - 1;

	if ( len > 5 || len == 0 )
	  error(ERROR3);
	else
	{
	  for ( i=(delim[1]+1); i < delim[2]; i++ )
	  {
	    if ( ! (isdigit(buf[i])))
	      baddigit++;
	    else if ( baddigit == 0 )
		gid=gid * 10 + (buf[i]) - '0';    /* converts ascii */
                                                  /* GID to decimal */
	  }
	  if ( baddigit > 0 )
	    error(ERROR3);
	  else if ( gid > 65535l || gid < 0l )
	      error(ERROR3);
	}

     /*  check that logname appears in the passwd file  */

	nptr = &tmpbuf[delim[2]];
	nptr++;
	while ( ( cptr = strchr(nptr,',') ) != NULL )
	{
	  *cptr=NULL;
	  if ( *nptr == NULL )
	  {
	    error(ERROR4a);
	    nptr++;
	    continue;
	  }
	  if (  getpwnam(nptr) == NULL )
	  {
	    badlognam=1;
	    error(ERROR4b);
	  }
	  nptr = ++cptr;
	  setpwent();
	}
	
  }
}

    /*	Error printing routine	*/

error(msg)

char *msg;
{
	if ( eflag==0 )
	{
	  fprintf(stderr,"\n\n%s",buf);
	  eflag=1;
	}

	if ( badchar != 0 )
	{
	  fprintf (stderr,"\t%d %s\n",badchar,msg);
	  badchar=0;
	  return;
	}
	else if ( baddigit != 0 )
	     {
		fprintf (stderr,"\t%s\n",msg);
		baddigit=0;
		return;
	     }
	     else if ( badlognam != 0 )
		  {
		     fprintf (stderr,"\t%s - %s\n",nptr,msg);
		     badlognam=0;
		     return;
		  }
		  else
		  {
		    fprintf (stderr,"\t%s\n",msg);
		    return;
		  }
}
