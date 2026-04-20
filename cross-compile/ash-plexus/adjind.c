/*
 * Adjust indentation.  The first arg is the old indentation amout,
 * the second arg is the new indentation amount, and the remaining
 * args are files to update.  Files are updated in place.  The original
 * version of a file named file.c is moved fo file.c~.
 *
 * Copyright (C) 1989 by Kenneth Almquist.
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose without fee is hereby granted,
 * provided that this copyright and permission notice is retained.
 */


#include <stdio.h>
#include <errno.h>

char tempfile[64];

FILE *ckfopen();

#define is_digit(c)	((unsigned)((c) - '0') <= 9)


main(argc, argv)
      char **argv;
      {
      int oldind, newind;
      double ratio;
      char **ap;

      if (argc < 3)
	    error("Usage: adjind old_indent new_indent file...");
      oldind = number(argv[1]);
      newind = number(argv[2]);
      if (oldind == 0)
	    error("Old indent cannot be zero");
      ratio = (double)newind / oldind;
      sprintf(tempfile, "/tmp/adjind%d", getpid());
      for (ap = argv + 3 ; *ap ; ap++) {
	    dofile(*ap, ratio);
      }
      done(0);
}



done(status) {
      exit(status);
}



dofile(fname, ratio)
      char *fname;
      double ratio;
      {
      register FILE *fp;
      register FILE *temp;
      register int c;
      register indent;
      double findent;
      char buf[1024];

      sprintf(tempfile, "%s.new", fname);
      fp = ckfopen(fname, "r");
      temp = ckfopen(tempfile, "w");
      for (;;) {	/* for each line of input, until EOF */
	    indent = 0;
	    for (;;) {
		  if ((c = getc(fp)) == ' ')
			indent++;
		  else if (c == '\t')
			indent = indent + 8 &~ 07;
		  else
			break;
	    }
	    findent = (double)indent * ratio;
	    indent = findent;
	    if (findent - indent > 0.5)
		  indent++;
	    while (indent >= 8) {
		  putc('\t', temp);
		  indent -= 8;
	    }
	    while (indent > 0) {
		  putc(' ', temp);
		  indent--;
	    }
	    if (c == EOF)
		  break;
	    putc(c, temp);
	    if (c != '\n') {
		  if (fgets(buf, 1024, fp) == NULL)
			break;
		  fputs(buf, temp);
	    }
      }
      fclose(fp);
      if (ferror(temp) || fclose(temp) == EOF)
	    error("Write error");
      sprintf(buf, "%s~", fname);
      movefile(fname, buf);
      movefile(tempfile, fname);
}



/*
 * Rename a file.  We do it with links since the rename system call is
 * not universal.
 */

movefile(old, new)
      char *old, *new;
      {
      int status;

      if ((status = link(old, new)) < 0 && errno == EEXIST) {
	    unlink(new);
	    status = link(old, new);
      }
      if (status < 0)
	    error("link failed");
      if (unlink(old) < 0)
	    perror("unlink failed");
}



FILE *
ckfopen(file, mode)
      char *file;
      char *mode;
      {
      FILE *fp;

      if ((fp = fopen(file, mode)) == NULL) {
	    fprintf(stderr, "Can't open %s\n", file);
	    done(2);
      }
      return fp;
}


int
number(s)
      char *s;
      {
      register char *p;

      for (p = s ; is_digit(*p) ; p++);
      if (p == s || *p != '\0') {
	    fprintf(stderr, "Illegal number: %s\n", s);
	    done(2);
      }
      return atoi(s);
}



error(msg)
      char *msg;
      {
      fprintf(stderr, "%s\n", msg);
      done(2);
}
