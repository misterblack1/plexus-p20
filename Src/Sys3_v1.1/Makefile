ARGS =
LEV1 =	graphics
LEV2 =	glib stat dev toc gutil
DOC =	whatis
BIN1 =	/usr/bin
BIN2 =	/usr/bin/graf
LIB =	/usr/lib/graf

all:;	make BIN1=$(BIN1) BIN2=$(BIN2) $(LEV1) $(LEV2) $(DOC)

graphics:;	cp graphics.sh $(BIN1)/graphics
		chmod 755 $(BIN1)/graphics

$(LEV2):;	cd $@.d; make BIN=$(BIN2) $(ARGS)
$(DOC):; -mkdir $(LIB)/$@; chmod 755 $(LIB)/$@
	cd $@.d; LIB=$(LIB) ./Install *.w
	cp $@.sh $(BIN2)/$@
	chmod 755 $(BIN2)/$@

install:;
clobber:;
