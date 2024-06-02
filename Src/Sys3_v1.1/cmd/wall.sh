trap "rm -f /tmp/$$; exit 0" 0 1 2 15
a="`who am i`"
a=`expr "$a" : '\([^ ]*\)'`
echo ''Broadcast Message from $a ''>/tmp/$$
cat>>/tmp/$$
who^sed -e 's/^[^ ]* *\([^ ]*\).*/cat \/tmp\/'$$' >\/dev\/\1 \&sleep 2/' | sh
sleep 30
