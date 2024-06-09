
#	MMSID (@(#)mm.sh	1.16)
if test $# = 0;		then help text3;	exit 1;	fi
trap 'trap 0; if test "$m" = 0; then mesg -y; fi; exit $z' 0
trap 'trap 0; if test "$m" = 0; then mesg -y; fi; exit 1' 1 2 3 15
if test -t;	then mesg >/dev/null;	m=$?;	mesg -n; else m=1; fi
PATH=/bin:/usr/bin:/usr/lbin
TERM=${TERM-450};  t=0;  v=0;  y=0;  g="";  h="-h";  w="";	u="-cm"
while test -n "$1" -a ! -r "$1"
do case $1 in
	-T*)	TERM=`echo $1 | sed "s/-T//"` ;;
	-12)		y=1 ;;
	-e)		e=neqn ;;
	-t)		f=tbl ;;
	-c)		c=c ;;
	-E)		a="$a -e" ;;
	-y)		u=-mm ;;
	-)		break ;;
	*)		a="$a $1" ;;
   esac
   shift
done
if test -z "$1";	then echo "mm: no input file";		z=1;	exit;	fi
case "$TERM" in
	300|300s|450|37|300-12|300s-12|450-12|4000A|382|X)	;;
	4014|tek)	g="|4014" ;;
	1620)	TERM=450 ;;
	1620-12)	TERM=450-12 ;;
	hp|2621|2640|2645)	v=1;	c=c;	a="-u1 $a";	g="|hp";	TERM=hp ;;
	735|745|43|40/4|40/2)	v=1;	c=c ;;
	2631|2631-c|2631-e)	v=3;	c=c ;;
	*)	TERM=lp;	v=1;	c=c ;;
esac
if test \( "$y" = 1 \) -a \( "$TERM" = 300 -o "$TERM" = 300s -o "$TERM" = 450 \)
	then TERM="$TERM"-12
fi
if test "$c" = c
then
	case "$TERM" in
		300|300s|450|4014|tek)	g="|col -f|greek -T$TERM" ;;
		300-12|300s-12|450-12)	g="|col -f|greek -T$TERM";	w="-rW72" ;;
		37|4000A|382|X)	g="|col -f" ;;
		hp)	g="|col|hp" ;;
		2631-c|2631-e)	g="|col -p" ;;
		735|745)	g="|col -x" ;;
		43)	g="|col -x";	w="-rW75" ;;
		40/4|40/2)	g="|col -b" ;;
		lp|2631)	g="|col" ;;
	esac
	h=""
	if test "$v" = 0;	then v=2;	fi
fi
d="$*"
if test "$d" = "-";	then shift;		d="";	fi
if test -n "$f"
	then	if test "$v" = 1 -o "$v" = 3
			then f="tbl -TX $*|"
			else f="tbl $*|"
		fi
		d=""
fi
if test -n "$e"
	then	if test -n "$f"
			then e="neqn|"
			else e="neqn $*|";	d=""
		fi
fi
if test "$TERM" = 4014 -o "$TERM" = hp;		then v=2;	fi
case "$v" in
	1)	TERM=lp ;;
	2)	TERM=37 ;;
esac
eval "$f $e nroff $u $w -T$TERM $h $a $d $g";	z=0;	exit
