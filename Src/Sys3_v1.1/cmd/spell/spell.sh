
#	spell program
# SCCS: @(#)spell.sh	1.6
# B flags, D_SPELL dictionary, F files, H_SPELL history, S_SPELL stop, V data for -v
H_SPELL=${H_SPELL-/usr/lib/spell/spellhist}
T=/tmp/spell.$$
V=/dev/null
F= B=
trap "rm -f $T*; exit" 0 1 2 13 15
for A in $*
do
	case $A in
	-v)	B="$B -v"
		V=${T}a ;;
	-a)	: ;;
	-b) 	D_SPELL=${D_SPELL-/usr/lib/spell/hlistb}
		B="$B -b" ;;
	-x)	B="$B -x" ;;
	*)	F="$F $A"
	esac
	done
deroff -w $F |\
  sort -u |\
  /usr/lib/spell/spellprog ${S_SPELL-/usr/lib/spell/hstop} $T |\
  /usr/lib/spell/spellprog ${D_SPELL-/usr/lib/spell/hlista} $V $B |\
  sort -u +0f +0 - $T |\
  tee -a $H_SPELL
who am i >>$H_SPELL 2>/dev/null
case $V in
/dev/null)	exit
esac
sed '/^\./d' $V | sort -u +1f +0
