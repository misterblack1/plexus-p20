a=`who am i`
b=`expr "$a" : '.* \(co\)nsole' \| "$a" : '.* tty\(..\)' \| noterm`
nice --20 ps -ef^sed -e '1d' \
 -e '/INIT/d' \
 -e '2s/.*/kill -'${1-9}' \\/' \
 -e '/^.*[0-9]*  *[0-9]*.* '$b' /d' \
 -e '/^ *'$$' /d' \
 -e '/shutdown/d' \
 -e '/\/etc\/rc/d' \
 -e '/killall/d' \
 -e '/root .* ps -ef/d' \
 -e '/root .*sed -e/d' \
 -e 's/....... *\([0-9][0-9]*\).*/ \1 \\/' \
 -e '$s/\\$//' ^ sh 2>&1 >/dev/null
