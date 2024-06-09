if [ ! -r ${1-/sys3} ]
then
	echo namelist not readable
	exit 1
fi

a=`ps -e -n ${1-/sys3}|grep errdemon|grep -v grep|sed -e "s/\(......\).*/kill -15 \1 > \/dev\/null /"`
if [ "$a" ]
then
	if eval $a
	then
		:
	else
		echo Not root
	fi
else
	echo No demon active
	exit 1
fi
exit 0
