T=/usr/tmp/lint.$$				#intermediate file
H=/usr/tmp/hlint$$				#header buffer file
L=/usr/lib	LL=/usr/lib			#directories
PATH=/bin:/usr/bin 
O="-C -Dlint"  X=  P=unix			#default parameters
trap "rm -f $T $H; exit" 0 1 2 3 15		#trap on exit
for A in $*					#process arguments
do
	case $A in
	-[IDU]*)	O="$O $A" ;;		#option(s) for C preprocessor
	-l*)	cat $LL/llib$A.ln >>$T ;;	# '-l ' parameter specified
	-*n*)	P=""  X="$X$A" ;; 		# '-n' option specified
	-*p*)	P="port"  X="$X$A" ;;		# '-p' option specified
	-*)	X="$X$A" ;;			#other lint parameter(s)
	*)	(/lib/cpp $O $A | $L/lint1 $X -H$H $A >>$T)2>&1 ;;
						#feed file through preprocessor
						#then through lint first pass
						#note that file name is passed
						#to lint1 as an argument
	esac
done
case $P in					#select appropriate library
	unix)	cat $LL/llib-lc.ln >>$T ;;
	port)	cat $LL/llib-port.ln >>$T ;;
esac
if [ -s $H ]
then
	$L/lint2 -T$T $X -H$H			#lint second pass
fi
