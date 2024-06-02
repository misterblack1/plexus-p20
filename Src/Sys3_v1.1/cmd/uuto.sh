mysys=`uname -n`
mesg="Usage: $0 [-m -p] fules remote!user"
if test  $# -lt 1
	then
		echo $mesg
		exit
fi
trap "trap '' 0; exit" 0 1 2 13 15
copy=""
#	get options
while true
do
	case $1 in
	 -m | -r* | -x*)	a="$a $1" ;;
	 -p)	copy=1 ;;
	 -d | -n*)	;;
	 -*)	echo $mesg; exit;;
	 *)	break ;;
	esac
	shift
done
#	get file names
while test $#  -gt 1
do
	if test -r "$1" -a -f "$1"
		then f="$f $1"
	elif test -r "$1" -a -d "$1"
		then d="$d $1"
		elif test "$UUP" = ""
		then echo "$1: file/directory not found"; exit
	fi
	shift
done
#	the recipient arg: remote!user
remote=`expr $1 : '\(.*\)!'`
user=`expr $1 : '.*!\(.*\)' \| $1`
if test -z "$copy"
then a="$a -c"
fi
	a="$a -d -n$user"
error=1
if test -n "$d" -a -n "$user"
then
	for i in $d
	do
	( cd $i; UUP="$UUP/$i" uuto * $1 )
	error=0
	done
fi
if test -n "$f" -a -n "$user"
then
	uucp $a $f $remote!~/receive/$user/$mysys$UUP/
	error=0
fi
if test error = 1
then 
	echo $mesg
	exit 2
fi
