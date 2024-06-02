#       "periodically check the size of /usr/adm/pacct"
#       "if over $1 blocks (1000) default, execute turnacct switch"
#       "should be run as adm"
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
export PATH

max=${1-1000}
cd /usr/adm

cp /dev/null cklock
chmod 400 cklock
ln cklock cklock1
if test $? -ne 0 ; then
	exit
fi

#	"dont run at 4am when daily accounting is running"
d=`date +%H%M`
if test $d -gt 0345 -a $d -lt 0415 ; then
	rm -f cklock*
	exit
fi

if test -r pacct ; then
	a="`ls -ls pacct`"
	sleep 1
	b="`ls -ls pacct`"
	if test "$a" != "$b" ; then
		set ${b}
		if test ${max} -lt ${1}; then
			turnacct switch
		fi
	fi
fi
rm -f cklock*
exit
