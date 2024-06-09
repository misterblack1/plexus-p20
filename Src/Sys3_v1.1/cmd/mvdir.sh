if [ $# != 2 ]
then
  echo "Usage: mvdir fromdir newname" 
  exit 2
fi
if [ $1 = . ]
then
	echo "mvdir: cannot move `.'"
	exit 2
fi
f=`basename $1`
t=$2
if [ -d $t ]
then
	t=$t/$f
fi
if [ -f $t  -o -d $t ]
then
  echo $t exists
  exit 1
fi
if [  ! -d $1 ]
then
  echo $1 must be a directory
  exit 1
fi
a=`expr $t : $1/`
if [ $a != 0 ]
then
  echo arguments have common path
  exit 1
fi
T=`expr $t : '\(.*\)/' \| .`
if /etc/link $1 $t ; then
	:
else
	echo "Cannot link to" $t 
	exit 2
fi
/etc/unlink $1
/etc/unlink $t/..
/etc/link $T $t/..
