PATH=/bin:/usr/bin
tmp=/tmp/cal$$
trap "rm $tmp; trap '' 0; exit" 0 1 2 13 15
/usr/lib/calprog >$tmp
case $# in
0)
	egrep -f $tmp calendar;;
*)
	sed '
		s/\([^:]*\):.*:\(.*\):[^:]*$/y=\2 z=\1/
	' /etc/passwd \
	| while
		read x
	do
		eval $x
		egrep -f $tmp $y/calendar 2>/dev/null \
		| mail $z
	done
esac
