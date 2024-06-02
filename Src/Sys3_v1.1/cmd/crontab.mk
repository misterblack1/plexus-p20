TESTDIR = .
FRC =
INS = /etc/install
INSDIR = /usr/lib/ 

all: crontab

crontab: crontab.sh $(FRC)
	cp crontab.sh $(TESTDIR)/crontab
	> $(TESTDIR)/cronlog

install: all
	$(INS) -o $(TESTDIR)/crontab $(INSDIR)
	$(INS) $(TESTDIR)/cronlog $(INSDIR)

clean:

clobber: clean
	-rm -f $(TESTDIR)/crontab $(TESTDIR)/cronlog

FRC:
