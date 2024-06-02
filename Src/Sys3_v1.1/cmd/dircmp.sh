PATH=/bin:/usr/bin
trap "rm -f /usr/tmp/dc$$*;exit" 1 2 3 15
while [ $# -ge 2 ]
do
	case $1 in
	-d)	Dflag="yes"
		shift
		;;
	-s)	Sflag="yes"
		shift
		;;
	-*)	echo unknown option
		exit 3
		;;
	 *)	D1=$1 D2=$2
		break
		;;
	esac
done
if [ $# -lt 2 ]
then echo $0: usage: $0 -s -d directory directory
     exit 1
elif [ ! -d "$D1" ]
then echo $D1 not a directory !
     exit 2
elif [ ! -d "$D2" ]
then echo $D2 not a directory !
     exit 2
fi
D0=`pwd`
cd $D1
find . -print | sort > /usr/tmp/dc$$a
cd $D0
cd $D2
find . -print | sort > /usr/tmp/dc$$b
comm /usr/tmp/dc$$a /usr/tmp/dc$$b | sed -n \
	-e "/^		/w /usr/tmp/dc$$c" \
	-e "/^	[^	]/w /usr/tmp/dc$$d" \
	-e "/^[^	]/w /usr/tmp/dc$$e"
rm -f /usr/tmp/dc$$a /usr/tmp/dc$$b
pr -h "$D1 only and $D2 only" -m /usr/tmp/dc$$e /usr/tmp/dc$$d
rm -f /usr/tmp/dc$$e /usr/tmp/dc$$d
sed -e s/..// < /usr/tmp/dc$$c > /usr/tmp/dc$$f
rm -f /usr/tmp/dc$$c
cd $D0
while read a
do
	if [ -d $D1/$a ]
	then if [ "$Sflag" != "yes" ]
	     then echo "directory	$a"
	     fi
	elif [ -f $D1/$a ]
	then cmp -s $D1/$a $D2/$a
	     if [ $? = 0 ]
	     then if [ "$Sflag" != "yes" ]
		  then echo "same     	$a"
		  fi
	     else echo "different	$a"
		  if [ "$Dflag" = "yes" ]
		  then diff $D1/$a $D2/$a | pr -h "diff of $a in $D1 and $D2" >> /usr/tmp/dc$$g
		  fi
	     fi
	elif [ "$Sflag" != "yes" ]
	then echo "special  	$a"
	fi
done < /usr/tmp/dc$$f | pr -r -h "Comparison of $D1 $D2"
if [ "$Dflag" = "yes" ]
then cat /usr/tmp/dc$$g
fi
rm -f /usr/tmp/dc$$*
