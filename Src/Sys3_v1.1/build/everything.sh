:	"[cmd=<command>] [lib=<library>] [uts=<uts#>] [games=<game>] \
:		everything.sh [strip]" 
:
:	Everything.sh makes the headers, libraries, uts, commands, games,
:	and kludges in that order.  It makes everything if cmd, lib,
:	games and uts are null.  If any of them are non-null,
:	it starts with the named make entity, e.g., lib=libplot,
:	and continues from there.
:	"<command>" should be a file or directory in /usr/src/cmd directory.
:	"<library>" should be one of the directories in /usr/src/lib.
:	"<uts#>" should be 16 or 32
:	"<game>" should be a file or directory in /usr/src/games.
VER=${VER-""}
REL=${REL-1.0}

allnull='if test -z "$lib" -a -z "$uts" -a -z "$cmd" -a -z "$kludge" \
-a -z "$games"; then echo true; else echo false ; fi'

if test `eval $allnull` = "false"; then echo "====== RESTART\n\n"; fi

:	=== HEADER FILES ===
if test `eval $allnull` = true; then 
	sh /usr/src/:mkhead \*
fi

:	=== LIBRARIES ===
:	Keep liblist is alphanumeric order
liblist="libPW libc libl libm libplot liby"
if test -n "$lib"; then 
	sh /usr/src/:mklib `echo " " $liblist " " ^ sed "s/.* $lib / $lib /"`
	lib=
elif test `eval $allnull` = true; then
	sh /usr/src/:mklib `echo $liblist`
fi

:	=== UTS ===
:	Keep in alphanumeric order
echo COPYING OVER CCOM
mv /usr/lib/ccom /usr/lib/ccom.sav
cp /usr/src/build/ccom.uts /usr/lib/ccom
chmod 755 /usr/lib/ccom
utslist="16 32"
if test -n "$uts"; then
	for i in `echo " " $utslist " " ^ sed "s/.* $uts / $uts /"` ; do
		REL=$REL VER=$VER sh /usr/src/:mkuts $i 
		uts=
	done
elif test `eval $allnull` = true; then
	for i in `echo "$utslist"` ; do
		REL=$REL VER=$VER sh /usr/src/:mkuts $i 
	done
fi
mv /usr/lib/ccom.sav /usr/lib/ccom
	
:	=== CMD ===
cmdlist=`ls /usr/src/cmd`
cmdlist=`echo places as cc ld yacc $cmdlist`
if test -n "$cmd"; then
	cmdlist=`echo " " $cmdlist " " ^ sed "s/.* $cmd / $cmd /"`
	cmd=
elif test `eval $allnull` = false; then
	cmdlist=
fi
if test "$1" = strip -a -n "$cmdlist"; then
	LDFLAGS='-n -s' sh /usr/src/:mkcmd $cmdlist
elif test -n "$cmdlist"; then
	sh /usr/src/:mkcmd $cmdlist
fi

:	=== GAMES ===
gameslist=`ls /usr/src/games`
if test -n "$games"; then 
	sh /usr/src/:mkgames `echo " " $gameslist " " ^ \
		sed "s/.* $games / $games /"`
	games=
elif test `eval $allnull` = true; then
	sh /usr/src/:mkgames `echo $gameslist`
fi
:	=== KLUDGES ===
:	The following cannot be made via above mechanism

set -x
cd /usr/src/cmd

rm -f /bin/STTY
ln /bin/stty /bin/STTY

cp passwd.etc  /etc/passwd
chmod 644 /etc/passwd; chown root /etc/passwd; chgrp bin /etc/passwd

cp lib.b.sh lib.b
/etc/install -n /usr/lib lib.b /usr/lib

cp filesave.u.sh filesave.u
/etc/install -n /etc filesave.u /etc

cp tapesave.u.sh tapesave.u
/etc/install -n /etc tapesave.u /etc

cd /usr/src/cmd/dc
cc -i dc.c -o dc
/etc/install -n /usr/bin dc

ln /usr/bin/300s /usr/bin/300S

sync

echo STAND GAMES NOT BEING MADE
