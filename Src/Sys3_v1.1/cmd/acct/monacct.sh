#	"this procedure should be run periodically ( by mounth or fiscal )"
a=/usr/adm
b=acct/nite
c=$a/$b
e=acct/sum
f=acct/fiscal
PATH=:/usr/lib/acct:/bin:/usr/bin:/etc
export PATH


if test $# -ne 1; then
	echo "usage: monacct fiscal-number"
	exit
fi

fiscal=$1

cd $a

#	"move summary tacct file to fiscal directory"
mv ${e}/tacct ${f}/tacct${fiscal}

#	"delete the daily tacct files"
rm -f ${e}/tacct????

#	"restart summary tacct file"
nulladm ${e}/tacct

#	"move summary cms file to fiscal directory
mv ${e}/cms ${f}/cms${fiscal}

#	"restart summary cms file"
nulladm ${e}/cms

#	"remove old prdaily reports"
rm -f ${e}/rprt*

#	"produce monthly reports"
prtacct ${f}/tacct${fiscal} > ${f}/fiscrpt${fiscal}
acctcms -a -s ${f}/cms${fiscal} |  \
pr -h "TOTAL COMMAND SUMMARY FOR FISCAL ${fiscal}" >> ${f}/fiscrpt${fiscal}
pr -h "LAST LOGIN" -3 ${e}/loginlog >> ${f}/fiscrpt${fiscal}

#	"add commands here to do any charging of fees, etc"
exit
