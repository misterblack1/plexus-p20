CC = cc
INSDIR = /etc

CFLAGS = -O
FFLAG =
LDFLAGS = -s

all:	prfld prfdc prfpr prfsnap prfstat

install:
	make -f profiler.mk  $(ARGS) FFLAG="$(FFLAG)" CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)"


prfld:		prfld.c
	$(CC) $(CFLAGS) $(FFLAG) $(LDFLAGS) -o prfld prfld.c
	cp prfld $(INSDIR)/prfld
	-chmod 775 $(INSDIR)/prfld

prfdc:		prfdc.c
	$(CC) $(CFLAGS) $(FFLAG) $(LDFLAGS) -o prfdc prfdc.c
	cp prfdc $(INSDIR)/prfdc
	-chmod 775 $(INSDIR)/prfdc

prfpr:		prfpr.c
	$(CC) $(CFLAGS) $(FFLAG) $(LDFLAGS) -o prfpr prfpr.c
	cp prfpr $(INSDIR)/prfpr
	-chmod 775 $(INSDIR)/prfpr

prfsnap:	prfsnap.c
	$(CC) $(CFLAGS) $(FFLAG) $(LDFLAGS) -o prfsnap prfsnap.c
	cp prfsnap $(INSDIR)/prfsnap
	-chmod 775 $(INSDIR)/prfsnap

prfstat:	prfstat.c
	$(CC) $(CFLAGS) $(FFLAG) $(LDFLAGS) -o prfstat prfstat.c
	cp prfstat $(INSDIR)/prfstat
	-chmod 775 $(INSDIR)/prfstat

clean:
	-rm -f prfdc prfld prfpr prfsnap prfstat *.o

clobber:	clean
