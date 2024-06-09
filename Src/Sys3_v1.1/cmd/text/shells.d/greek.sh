
#	GREEKSID (@(#)greek.sh	1.6)
TERM=${1--T$TERM}
case $TERM in
-T450|-T450-12|-T1620|-T1620-12)	exec 450;;
-T300)	exec 300;;
-T300-12)	exec 300 +12;;
-T300s)	exec 300s;;
-T300s-12)	exec 300s +12;;
-Thp|-T2621|-T2645|-T2640)	exec hp;;
-T4014|-Ttek)	exec 4014;;
"")	echo 'usage: greek [ -Tterminal ]  ($TERM must be set if arg omitted)';;
*)	echo "$TERM not implemented"
	exit 1;;
esac
