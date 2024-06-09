: '/etc/whodo -- combines info from who(1) and ps(1)'
trap  "rm -f /tmp/$$*" 1 2 3
ps -a>/tmp/$$b&
date
/bin/who>/tmp/$$a
ed - /tmp/$$a<<\!
g/^/s/^\(.........\)\(........\).......\(.*\)/\2 \1 \3/
g/^tty/s/^tty\(..\)../\1/
g/^console/s//co/
w
!
uname
wait
ed - /tmp/$$b<<\!
1d
g/^/s/^\(......\).\(..\)\(.*\)/\2-  \1\3/
g/-sh$/s/-/+/
g/-rsh$/s/-/*/
w
!
sort /tmp/$$a /tmp/$$b >/tmp/$$c
ed - /tmp/$$c<<\!
g/^..+/s/.....//\
s/\([0-9]\)  *.*/\1/\
.-1,.j
g/^..\*/s/.....//\
s/\([0-9]\)  *.*/\1/\
.-1s/ /r/\
j
g/^\(..\)- /s//  \1/
1,$p
Q
!
rm -f /tmp/$$*
