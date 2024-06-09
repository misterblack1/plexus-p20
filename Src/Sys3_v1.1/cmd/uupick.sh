
# sys: system; user: login name;  cdir: current directory;
# tdir: temporary directory; pu: PUBDIR/receive/user;
cdir=`pwd`
trap "exit" 1 2 13 15
# get options
while test $# -gt 1
do
	case $1 in
	 -s)	sys=$2 ;;
	 *)	echo "Usage: $0 [-s sysname]"; exit
	esac
	shift; shift
done
user=`who am i | sed -n -e 's/ .*//p'`
pu=/usr/spool/uucppublic/receive/$user
if test -d $pu -a -s $pu
then
for i in `ls $pu`
do
	if test $sys
	then	if test $sys != $i;  then continue;  fi
	fi
	if test -d $pu/$i -a -s $pu/$i
	then
		cd $pu/$i
		for j in `ls`
		do
			if test -d $j
			then echo "from system $i: directory $j"
			else echo "from system $i: file $j"
			fi
			while true
			do
			  echo '?'
			  if read cmd dir
			  then
				trap ": ;;" 1
				case $cmd in
				 d)	rm -fr $j ; break ;;
				 "")	break ;;
				 m)	if test $dir
					then tdir=$dir
					else tdir=$cdir
					fi
					find $j -print | cpio -pdmul $tdir
					if test $? -eq 0
					then rm -fr $j
					fi
					break ;;
				 a)	if test $dir
					then tdir=$dir
					else tdir=$cdir
					fi
					find * -print | cpio -pdmul $tdir
					if test $? -eq 0
					then rm -fr *
					fi
					break 2 ;;
				 p)	if test -d $j
					then cat $j
					else find . -print
					fi ;;
				 q)	break 3 ;;
				 !*)	ex=`expr "$cmd $dir" : '!\(.*\)'`
					tdir=`pwd`
					cd $cdir
					sh -c "$ex"
					cd $tdir
					echo '!' ;;
				 *)	echo "usage: [d][m dir][a dir][p][q]\c"
					echo "[cntl-d][!cmd][*][new-line]" ;;
				esac
				trap 1
			  else	break 3
			  fi
			done
		done
	fi
done
fi
