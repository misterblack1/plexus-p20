/*LINTLIBRARY*/
#include	<stdio.h>
#define ERR(s, c)	if(opterr){\
	fputs(argv[0], stderr);\
	fputs(s, stderr);\
	fputc(c, stderr);\
	fputc('\n', stderr);}

int	XXsp = 1;
int	opterr = 1;
int	optind = 1;
int	optopt;
char	*optarg;
char	*strchr();

int
getopt (argc, argv, opts)
char **argv, *opts;
{
	register c;
	register char *cp;

	if (XXsp == 1)
		if (optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return EOF;
		else if (strcmp(argv[optind], "--") == NULL) {
			optind++;
			return EOF;
		}
	optopt = c = argv[optind][XXsp];
	if (c == ':' || (cp=strchr(opts, c)) == NULL) {
		ERR (": illegal option -- ", c);
		if (argv[optind][++XXsp] == '\0') {
			optind++;
			XXsp = 1;
		}
		return '?';
	}
	if (*++cp == ':') {
		if (argv[optind][XXsp+1] != '\0')
			optarg = &argv[optind++][XXsp+1];
		else if (++optind >= argc) {
			ERR (": option requires an argument -- ", c);
			XXsp = 1;
			return '?';
		} else
			optarg = argv[optind++];
		XXsp = 1;
	}
	else {
		if (argv[optind][++XXsp] == '\0') {
			XXsp = 1;
			optind++;
		}
		optarg=NULL;
	}
	return c;
}
