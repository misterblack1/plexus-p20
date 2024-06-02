INS = :
FILES = arithmetic.o back.o bj.o craps.o fish.o fortune.o hangman.o mail.o \
	moo.o number.o psych.o quiz.o random.o ttt.o wump.o

install:
	make -f games.mk all INS="/etc/install -f /usr/games"

all: $(FILES)

.c .c.o:
	cc $(LDFLAG) $(CFLAGS) -o $* $< -lm
	$(INS) $*

psych.o psych: psych.c
	echo $* does not compile

clean:
	rm -f *.o

clobber: clean
	rm -f arithmetic back bj craps fish fortune hangman mail \
		moo number psych quiz random ttt wump
