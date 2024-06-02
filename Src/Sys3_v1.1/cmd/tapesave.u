# 'Volcopy backup to tape:'
#
#  The following script is to be used as
#  a sample only.  It is the script in
#  use on USG System A (mhtsa) at the time
#  of this release, and does not necessarily
#  reflect the needs of another site.
#
# 'USG site only!'
echo 'Enter file system name (e.g., usr):'
read f
echo 'Enter device name (e.g., /dev/rrp10):'
read d
echo 'Enter pack volume label (e.g., p0045):'
read v
: 'This is site dependent!!!'
# echo 'Enter tape drive number (e.g., 0, 1,...):'
# read t
# if test $t = 0
# then
#   t=/dev/rmt2
# elif test $t = 1
# then
#   t=/dev/rmt3
# else
#   echo 'Not one of our tape drives'
#   exit 1
# fi
t=/dev/rmt1
echo 'Enter tape volume label (e.g., t0005):'
read l

/etc/labelit  $t
if test  $? -gt 0
then
  /etc/labelit $t $f  $l -n
  if test $? -gt 0
  then
    exit 1
  fi
fi
/etc/volcopy $f $d $v $t $l
