T=/tmp/$$ PATH=/bin:/usr/bin O= OTWO= S= N=yes OPT= ret=0 prof= warn= code=
L=/lib COPT=/lib/c2
LC="$L/crt0.o" LIBS=" $L/libc.a" NAS=as LD=ld CCOMP=/usr/lib/ccom
MLC="$L/mcrt0.o" CPP=/lib/cpp E=
ECHO=echo
trap "rm -f $T.*; exit 1" 1 2 3 15
trap 'rm -f $T.*; exit $ret' 0
for A
do	case $A in
	*.c)	B=`basename $A .c`
		$ECHO "$B.c"
		case "$E$S$OPT" in
		-S)	if $CPP $O $A $T.i &&
			   $CCOMP  $warn $code $prof <$T.i $OTWO >$B.s; then
				: OK
			else
				ret=$?
			fi ;;
		-Syes)	if $CPP $O $A $T.i &&
			   $CCOMP  $warn $code $prof <$T.i $OTWO >$T.s &&
			   $COPT $T.s $B.s; then
				: OK
			else
				ret=$?
			fi ;;
		yes)	if $CPP $O $A $T.i &&
			    $CCOMP  $warn $code $prof <$T.i $OTWO >$T.x &&
			    $COPT $T.x  $T.s &&
			    $NAS -u -o $B.o $T.s ; then
				ll="$ll $B.o"
			else
				N=no ret=$?
			fi ;;
		-E*)	$CPP $O $A;;
		-P*)	$CPP $O $A $B.i;;
		*)	if $CPP $O $A $T.i &&
			   $CCOMP  $warn $code $prof <$T.i $OTWO >$T.s &&
			    $NAS -u -o $B.o $T.s ; then
				ll="$ll $B.o"
			else
				N=no ret=$?
			fi ;;
		esac ;;

	-S)	N=no S=-S OTWO=-l ;;


	-O)	OPT=yes ;;

	-[IDU]*)	O="$O $A" ;;

	-c)	N=no ;;

	-)	ECHO=: ;;

	-[EP])	N=no O="$O $A" E=$A ECHO=: ;;

	*.s)	B=`basename $A .s`
		if echo "$B.s:" &&
		$NAS -u -o $B.o $A; then
			ll="$ll $B.o"
		else
			N=no ret=$?
		fi ;;

	-p)	prof=-Xp 
		LC=$MLC ;;

	-w)	warn=-Xw ;;

	-C*)	code=$A ;;

	*)	ll="$ll $A"
	esac
done
case $N in
	no)	: ;;
	*)	$LD -X $LC $ll $LIBS
	ret=$?
esac
