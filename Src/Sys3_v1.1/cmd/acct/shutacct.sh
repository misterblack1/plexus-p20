#	"shutacct [arg] - shuts down acct, called from /etc/shutdown"
#	"whenever system taken down"
#	"arg	added to /usr/adm/wtmp to record reason, defaults to shutdown"
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
a=${1-shutdown}
acctwtmp  $a  >>/usr/adm/wtmp
turnacct off
