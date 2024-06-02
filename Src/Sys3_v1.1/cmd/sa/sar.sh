TMP=/tmp
DATE=`date +%d`
RPT=/usr/adm/sa/sar$DATE
DFILE=/usr/adm/sa/sa$DATE
ENDIR=/usr/lib/sa
cd $ENDIR
$ENDIR/sarpt $DFILE
cat $TMP/sa.tmp* > $RPT
rm $TMP/sa.tmp*
find /usr/adm/sa \( -name 'sar*' -o -name 'sa*' \) -mtime +7 -exec rm {} \;
