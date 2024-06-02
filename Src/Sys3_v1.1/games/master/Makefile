CFLAGS = -O
OBJECTS = abbrev.o ematch.o equal.o gentab.o guess.o imatch.o\
	init.o instr.o main.o marksubs.o pack.o play1.o play2.o\
	prune.o randint.o read1.o review.o rguess.o unpack.o\
	yorn.o zcount.o

a.out:	$(OBJECTS)
	cc -n $(OBJECTS)

gentab.o:	gentab.c max.h

guess.o:	guess.c max.h
init.o:	init.c max.h
main.o:	main.c max.h
play2.o:	play2.c max.h
prune.o:	prune.c max.h
review.o:	review.c max.h
rguess.o:	rguess.c max.h
