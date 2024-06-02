LIB="-l2"
ERR="scc: bad conf lib"

CASEB="if pdp11; then LIB='-l2B'; shift; else echo $ERR; exit; fi"

case $1 in
+B)	eval "$CASEB";;
+A)	shift;;
+)	shift; case $1 in
	B)	eval "$CASEB";;
	A)	shift;;
	*)	LIB=;;
	esac;;
+*)	echo $ERR; exit;;
esac

#if vax
#then
#	cc -DSTANDALONE -e start -2x $*
#else
	cc -DSTANDALONE -2x $* $LIB
#fi
