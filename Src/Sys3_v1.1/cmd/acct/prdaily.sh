#	"prdaily	prints daily report"
#	"last command executed in runacct"
PATH=/usr/lib/acct:/bin:/usr/bin:/etc

c=/usr/adm/acct/nite

cd $c
m="`uname`"
(cat reboots; echo ""; cat lineuse) | pr -h "DAILY REPORT FOR $m"  

prtacct daytacct "DAILY USAGE REPORT FOR $m"  
pr -h "DAILY COMMAND SUMMARY" daycms
pr -h "MONTHLY TOTAL COMMAND SUMMARY" cms 
pr -h "LAST LOGIN" -3 ../sum/loginlog  
