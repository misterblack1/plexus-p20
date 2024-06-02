#	compress - compress the spell program log
#  SCCS: @(#)compress.sh	1.2

trap 'rm -f /usr/tmp/spellhist;exit' 1 2 3 15
echo "COMPRESSED `date`" > /usr/tmp/spellhist
grep -v ' ' /usr/lib/spell/spellhist | sort -fud >> /usr/tmp/spellhist
cp /usr/tmp/spellhist /usr/lib/spell
rm -f /usr/tmp/spellhist
