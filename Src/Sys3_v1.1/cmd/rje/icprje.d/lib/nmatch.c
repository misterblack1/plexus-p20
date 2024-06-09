static char *pat,lastmatch;

/*
 * Match attempts to match pattern with string.  Pattern
 * may contain operators or single character REs.  The following
 * are operators:
 *	-	Skip through string until the
 *		RE following the '-' is matched.
 *	$n	Same as above except the skipped string is
 *		saved in the nth argument after string (1<=n<=9).
 *	blank	Skip over white space in string.
 *	?x	Conditional.  Return(savecnt) if the last character matched
 *		in a [..] RE was x.
 *	'...'	Skip ahead looking for the string that matches
 *		the RE string in quotes
 *	\	Negates the affect of the above.
 * The following are single character REs:
 *	x	Any ascii character.  Must be matched by the
 *		next character in string.
 *	.	Matches any character in string.
 *	[...]	Matches any character contained in the brackets,
 *		- and ^ do not work.
 *	\	Negates the affect of the above.
 * Match goes until the end of pattern is reached or until
 * the string does not match the pattern.  The number of saved
 * strings ( from the $ op. ) is returned or -1 if there
 * is no more string but more pattern.
 */
nmatch(pattern,string,arg1)
char *pattern,*string,*arg1;
{
	register char *input;
	register char *save;
	register char *p;
	int savecnt;

	savecnt = 0;
	pat = pattern;
	input = string;
	while(*pat) {
		switch(*pat) {
		case '?':
			pat++;
			if(*pat++ == lastmatch)
				return(savecnt);
			break;
		case ' ':
			pat++;
			while(*input==' ' || *input=='\t')
				input++;
			if(*input == '\0')
				return(-1);
			break;
		case '-':
			pat++;
			while(rematch(*input++) < 0)
				if(*input == '\0')
					return(-1);
			break;
		case '$':
			pat++;
			save = (&arg1)[*pat++ - '1'];
			while(rematch(*input) < 0) {
				if(*input == '\0')
					return(-1);
				*save++ = *input++;
			}
			input++;
			savecnt++;
			*save = '\0';
			break;
		case '\'':
			pat++;
			p = pat;
			while(*pat != '\'') {
				pat = p;
				while(rematch(*input) == 0 && *pat != '\'')
					input++;
				if(*input == '\0')
					return(-1);
				if(pat == p)
					input++;
			}
			pat++;
			input++;
			break;
		case '\\':
			pat++;
		default:
			if(rematch(*input++) < 0)
				return(-1);
		}
	}
	return(savecnt);
}


/* Regular expression match.  Test if the RE at pat
 * matches the char c.  If it does advance pat past the
 * matched RE and return 0.  If it doesn't return -1.
 */
static
rematch(c)
register char c;
{
	register char *mark;
	register int flag;

	mark = pat;
	switch(*pat) {
	case '[':
		flag = 0;
		while(*pat != ']')
			if(*pat++ == c) {
				lastmatch = c;
				flag++;
			}
		if(flag) {
			pat++;
			return(0);
		}
		break;
	case '.':
		pat++;
		return(0);
	case '\\':
		pat++;
	default:
		if(*pat == c) {
			if(*pat)
				pat++;
			return(0);
		}
	}
	pat = mark;
	return(-1);
}
