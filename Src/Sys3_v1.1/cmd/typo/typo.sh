
#	TYPOSID(@(#)typo.sh	1.2)
if [ $# -lt 1 ]
then
	echo "typo: no input file"
	exit 1
fi
D=""
while [ 1 ]
do
A=$1
B="`expr substr "$1" 1 1`"
C="-"
if test $B = $C
then
case $A in
	-n)	D="$D $A";
		shift;;
	*)	echo "Invalid option ($A) - terminated"
		exit 1;;
esac
else
deroff $*^/usr/lib/typoprog $D^sort +0nr +1f
exit 1
fi
done
