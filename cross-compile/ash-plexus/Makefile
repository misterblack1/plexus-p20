CC=cc
CFLAGS=-O -DSHELL

OBJS=main.o options.o parser.o eval.o expand.o jobs.o redir.o exec.o \
 builtins.o cd.o miscbltin.o mail.o var.o input.o output.o nodes.o syntax.o \
 signames.o memalloc.o error.o trap.o show.o dirent.o mystring.o init.o

BLTIN_OBJS=bltin_catf.o bltin_echo.o bltin_line.o bltin_nlecho.o exprstub.o

all: ash

ash: $(OBJS) $(BLTIN_OBJS)
	$(CC) -o ash $(OBJS) $(BLTIN_OBJS) -lc

.c.o:
	$(CC) $(CFLAGS) -c $<

bltin_catf.o: bltin/catf.c
	cp bltin/catf.c bltin_catf.c
	$(CC) $(CFLAGS) -I. -Ibltin -c bltin_catf.c
	rm bltin_catf.c

bltin_echo.o: bltin/echo.c
	cp bltin/echo.c bltin_echo.c
	$(CC) $(CFLAGS) -I. -Ibltin -c bltin_echo.c
	rm bltin_echo.c

bltin_line.o: bltin/line.c
	cp bltin/line.c bltin_line.c
	$(CC) $(CFLAGS) -I. -Ibltin -c bltin_line.c
	rm bltin_line.c

bltin_nlecho.o: bltin/nlecho.c
	cp bltin/nlecho.c bltin_nlecho.c
	$(CC) $(CFLAGS) -I. -Ibltin -c bltin_nlecho.c
	rm bltin_nlecho.c

clean:
	rm -f *.o ash
