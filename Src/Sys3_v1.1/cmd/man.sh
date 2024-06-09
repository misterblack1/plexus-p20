
#	MANSID (@(#)man.sh	1.33)
if test $# = 0;		then help text2;	exit 1;	fi
trap 'trap 0; if test "$m" = 0; then mesg -y; fi; exit $z' 0
trap 'trap 0; if test "$m" = 0; then mesg -y; fi; exit 1' 1 2 3 15
if test -t;	then mesg >/dev/null;	m=$?;	mesg -n; else m=1; fi
PATH=/bin:/usr/bin:/usr/lbin;	y=0;	tbl="tbl";	u="-can";
troff=0;	cf=0;	sec=\?;	mdir=0
cmd= fil= opt= i= all= eopt=
for i
do case $i in
	[1-8])	sec=$i ;;
	-s)	if test "$cmd" = "";	then cmd=t;	fi
		troff=1;	opt="$opt -rs1";	eopt=-s9 ;;
	-t)	troff=1;	cmd=t ;;
	-Tst)	troff=1;	cmd=s ;;
	-T4014|-Ttek)	troff=1;	cmd=k ;;
	-Tvp)	troff=1;	cmd=v ;;
	-c)	c=c ;;
	-12)	y=1;;
	-d)	mdir=1 ;;
	-w)	cmd=w ;;
	-y)	u="-man" ;;
	-T*)	TERM=`echo $i | sed "s/-T//"` ;;
	-*)	opt="$opt $i" ;;
	*)	if test "$mdir" = 0
		then
			cd /usr/man
			fil1=`echo local/man$sec/$i.*`
			fil2=`echo man$sec/$i.*`
			case $fil1 in
				*\*)	case $fil2 in
						*\*)	echo man: $i not found 1>&2 ;;
						*)	all="$all $fil2" ;;
					esac ;;
				*)	case $fil2 in
						*\*)	all="$all $fil1" ;;
						*)	all="$all $fil1 $fil2" ;;
					esac ;;
			esac
		else
			if test ! -r "$i"
			then
				echo man:  $i not found 1>&2
			else
				all="$all $i"
			fi
		fi
   esac
done
if test "$cmd" = "w";	then echo $all;	z=0;	exit;	fi
if test $troff -eq 0
then
	v=0;	h="-h";	g=""
	if test "$TERM" = "";	then TERM=450; fi
	case "$TERM" in
		300|300s|450|37|300-12|300s-12|450-12|4000A|382|X)	;;
		4014|tek)	g="|4014" ;;
		1620)	TERM=450 ;;
		1620-12)	TERM=450-12 ;;
		hp|2621|2640|2645)	v=1;	c=c;	a="-u1 $a";	g="|hp -m";	TERM=hp ;;
		735|745|40/4|40/2)	v=1;	c=c ;;
		43)	v=1;	c=c;	opt="opt -rT1" ;;
		2631|2631-c|2631-e)	v=3;	c=c ;;
		*)	TERM=lp;	v=1;	c=c  ;
	esac
	if test \( "$y" = 1 \) -a \( "$TERM" = 300 -o "$TERM" = 300s -o "$TERM" = 450 \)
		then TERM="$TERM"-12
	fi
	if test "$c" = c
	then
		case "$TERM" in
			300|300s|450|300-12|300s-12|450-12|4014|tek)	g="|col -f|greek -T$TERM" ;;
			37|4000A|382|X)	g="|col -f" ;;
			hp)	g="|col|hp -m" ;;
			2631-c|2631-e)	g="|col -p" ;;
			735|745|43)	g="|col -x" ;;
			40/4|40/2)	g="|col -b" ;;
			lp|2631)	g="|col" ;;
		esac
		h=""
		if test "$v" = 0;	then v=2;	fi
	fi
	if test "$v" = 1 -o "$v" = 3;	then tbl="tbl -TX";	fi
	if test "$TERM" = 4014 -o "$TERM" = hp;	then v=2;	fi
	if test "$v" = 1;	then TERM=lp;	fi
	if test "$v" = 2;	then TERM=37;	fi
fi
for fil in $all
do
	ln=`line < $fil`
	ck=`echo $ln | sed "s/ .*//"`
	if test "$ck" = ".so"
	then
		rfil=`echo $ln | sed "s/.so //"`
	else
		rfil=$fil
	fi
	if test "$cmd" = s -o "$cmd" = t;	then cf=1;	fi
	prep="cat $fil"
	ln=`line < $rfil`
	ck=`echo $ln | sed "s/ .*//"`
	if test "$ck" = "'\\\""
	then
		case `echo $ln | sed "s/....//"` in
		c)	if test "$cf" = 1;	then prep="cw $fil";	fi ;;
		e)	if test $troff -eq 1
			then
				prep="eqn $eopt $fil"
			else
				prep="neqn $fil"
			fi ;;
		t)	prep="$tbl $fil" ;;
		ce | ec)
			if test "$cf" = 1
			then
				prep="cw $fil | eqn $eopt"
			elif test $troff -eq 1
			then
				prep="eqn $eopt $fil"
			else
				prep="neqn $fil"
			fi ;;
		ct | tc)
			if test "$cf" = 1
			then
				prep="cw $fil | $tbl"
			else
				prep="$tbl $fil"
			fi ;;
		et | te)
			if test $troff -eq 1
			then
				prep="$tbl $fil | eqn $eopt"
			else
				prep="$tbl $fil | neqn"
			fi ;;
		cet | cte | ect | etc | tce | tec)
			if test "$cf" = 1
			then
				prep="cw $fil | $tbl | eqn $eopt"
			elif test $troff -eq 1
			then
				prep="$tbl $fil | eqn $eopt"
			else
				prep="$tbl $fil | neqn"
			fi ;;
		esac
	fi
	d=`/usr/lib/manprog $rfil`
	O="";	o=""	# If GCOS, set O="-g"; o="| gcat -ph -f$fil"
	case $cmd in
		"")	eval "$prep | nroff -T$TERM $d$opt $h $a $u $g" ;;
		t)	eval "$prep | troff $O $d$opt $u $o" ;;
		s)	eval "$prep | troff -g $d$opt $u | gcat -st -f$fil" ;;
		k)	eval "$prep | troff $d -t$opt $u | tc" ;;
		v)	eval "$prep | troff $d -t$opt $u | vpr -t" ;;
	esac
done
z=0;	exit
