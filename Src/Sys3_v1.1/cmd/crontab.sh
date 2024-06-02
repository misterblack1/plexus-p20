0,10,20,30,40,50 * * * * /etc/eccdaemon /usr/adm/ecclog
0 * * * * date>/dev/console ; echo " " > /dev/console 
0 4 * * 1-6 /bin/su - adm -c "/usr/lib/acct/runacct 2> /usr/adm/acct/nite/fd2log"
0 2 * * 1-6 /bin/su - adm -c "/usr/lib/acct/sdisk"
0 * * * * /bin/su - adm -c "/usr/lib/acct/ckpacct"
0 9 * * 1-5 /usr/games/turnoff > /dev/null
0 18 * * 1-5 /usr/games/turnon > /dev/null
0 1 * * * /usr/bin/calendar -
56 * * * * /bin/su uucp -c "/usr/lib/uucp/uudemon.hr > /dev/null"
0 4 * * * /bin/su uucp -c "/usr/lib/uucp/uudemon.day > /dev/null"
30 5 * * 1 /bin/su uucp -c "/usr/lib/uucp/uudemon.wk > /dev/null"
