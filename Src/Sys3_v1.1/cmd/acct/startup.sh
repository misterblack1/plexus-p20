#	"startup (acct) - should be called from /etc/rc"
#	"whenever system is brought up"
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
acctwtmp `uname` >>/usr/adm/wtmp
turnacct on
#	"clean up yesterdays accounting files"
remove
