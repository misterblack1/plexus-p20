# FILE BACKUP SHELL PROCEDURE:
#
#  The following script is to be used as
#  a sample only.  It is the script in
#  use on USG System A (mhtsa) at the time
#  of this release, and does not necessarily
#  reflect the needs of another site.
#
export g l x a b
log=/etc/log ENDONE=root ENDTWO="done"
echo "The output of filesave.u is usr to pack one,"
echo "root, c, msrc, and mert to pack two."

trap 'echo "Want to go on ? (y or n):"
read g
if [ "$g" = "y" ]
then
	trap "exit" 2 3
	filesave.u restart
fi; exit' 2 3


if [ "$b" = "" ]
then
	f=""
else
	f="$1"
fi

while :
do

case $f in

restart )	echo "If first pack has been saved successfully"
		echo "do you want to go on to second pack ? (\`y' or \`n'):"
		read w
		if [ "$w" = "y" ]
		then
			f="$ENDONE"
		else
			f=usr
		fi
		;;

usr )  echo "Mount pack for  \`usr' on drive $x --"
		echo When ready, enter name of pack:
		read c
		if
			volcopy usr /dev/rrp11 $b /dev/rrp${x}1 $c &&
			volcopy root /dev/rrp0 $a /dev/rrp10 $b &&
			volcopy root /dev/rrp0 $a /dev/rrp${x}0 $c
		then
			echo "\n\nREMOVE PACK $c ON DRIVE $x\n\n"
			f="$ENDONE"
		else
			fail=1
		fi
		;;

root|c|root|msrc )	echo "Mount pack for \`root', etc. on drive $x --"
		echo When ready, enter name of pack:
		read d
		if
			volcopy root /dev/rrp0 $a /dev/rrp${x}0 $d &&
			volcopy c /dev/rrp1 $a /dev/rrp${x}1 $d &&
			volcopy msrc /dev/rrp2 $a /dev/rrp${x}2 $d &&
			volcopy mert /dev/rrp3 $a /dev/rrp${x}3 $d
		then
			echo "\n\nREMOVE $d FROM DRIVE $x\n\n"
			echo "Put the pack copies ($c and $d) on the shelf."
			f="$ENDTWO"
		else
			fail=1
		fi
		;;

"done" )
		fsck -n -t /tmp/fsck
		echo 'File save complete, enter your initials'
		read i
		echo Initials: $i >> $log/filesave
		echo "\n\nIf the file system check "\
		"was clean, run init 2"
		echo "IF THE FILE SYSTEM CHECK WAS ___NOT CLEAN,"
		echo "    (file size errors are OK)"
		echo "DO NOT RUN INIT 2, "\
		"LEAVE A MESSAGE FOR THE SYSTEM ADMINISTRATOR\n\n"
		exit
		;;

* )		echo "Is this a restart of the filesave(\`y' or \`n')?"
		read g
		if [ "$g" = "y" ]
		then
			echo "Enter filesystem backup you wish to restart at(e.g. \`usr' or \`root')."
			read l
		fi
		echo System U filesave:
		echo If anything goes wrong hit the delete key and get help!
		echo Enter number of spare drive:
		read x
		echo SPARE, $x > $log/filesave
		echo "Enter name of pack on drive 0 (e.g.: \`\`v0024''):"
		read a
		echo DRIVE 0 PACK, $a >> $log/filesave
		echo Enter name of pack on drive 1:
		read b
		echo DRIVE 1 PACK, $b >> $log/filesave
		if [ "$l" = "" ]
		then
			f=usr
		else
			f="$l"
		fi
		;;

esac

if [ "$fail" = "1" ]
then
	echo The last backup attempted failed.
	fail=0
fi

done
