#	"control process accounting (must be root)"
#	"turnacct on	makes sure it's on"
#	"turnacct off	turns it off"
#	"turnacct switch	switches pacct to pacct?, starts fresh one"
#	"/usr/adm/pacct is always the current pacct file"
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
cd /usr/adm
case "$1"  in
on)
	if test ! -r pacct
	then
		nulladm pacct
	fi
	accton pacct
	rc=$?
	;;
off)
	accton
	rc=$?
	;;
switch)
	if test -r pacct
	then
		i=1
		while test -r pacct$i
		do
			i="`expr $i + 1`"
		done
		mv pacct pacct$i
	fi
	nulladm pacct
	accton
	accton pacct
	rc=$?
	if test ${rc} -ne 0; then
		echo "accton failed"
		rm pacct
		mv pacct$i pacct
		exit ${rc}
	fi
	;;
*)
	echo "usage: turnacct on|off|switch"
	rc=1
	;;
esac
exit ${rc}
