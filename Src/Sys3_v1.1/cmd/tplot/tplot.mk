TESTDIR = .
INS = :
INSDIR =
FRC =
CFLAGS = -O
FFLAG =
IFLAG = -i

all: t4014 t300 t300s t450 vplot tplot

t4014:: driver.o
	$(CC) $(LDFLAGS) $(FFLAG) -o $(TESTDIR)/t4014 driver.o -l4014 -lm
t4014::
	$(INS) $(TESTDIR)/t4014 $(INSDIR)

t300::	driver.o
	$(CC) $(LDFLAGS) $(FFLAG) -o $(TESTDIR)/t300 driver.o -l300 -lm
t300::
	$(INS) $(TESTDIR)/t300 $(INSDIR)

t300s:: driver.o
	$(CC) $(LDFLAGS) $(FFLAG) -o $(TESTDIR)/t300s driver.o -l300s -lm
t300s::
	$(INS) $(TESTDIR)/t300s $(INSDIR)

t450::	driver.o
	$(CC) $(LDFLAGS) $(FFLAG) -o $(TESTDIR)/t450 driver.o -l450 -lm
t450::
	$(INS) $(TESTDIR)/t450 $(INSDIR)

vplot::	vplot.o banner.o
	$(CC) $(LDFLAGS) $(FFLAG) $(IFLAG) -o $(TESTDIR)/vplot vplot.o banner.o

vplot::
	$(INS) $(TESTDIR)/vplot $(INSDIR)

tplot::	tplot.sh
	cp tplot.sh $(TESTDIR)/tplot
tplot::
	$(INS) $(TESTDIR)/tplot $(INSDIR)

driver.o: driver.c $(FRC)

install:
	 make -f tplot.mk FFLAG=$(FFLAG) IFLAG=$(IFLAG) all INS="/etc/install -n /usr/bin" LDFLAGS="$(LDFLAGS)"

clean:
	 -rm -f *.o

clobber: clean
	 -cd $(TESTDIR); rm -f t4014 t300 t300s t450 vplot tplot


FRC:
