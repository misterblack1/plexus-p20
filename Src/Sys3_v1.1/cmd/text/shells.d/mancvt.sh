
#	MANCVT(@(#)mancvt.sh	1.3)
while [ $1 ]
do
ed - $1 <<\!
g/^\.th/s//.TH/
g/^\.sh/s//.SH/
g/^\.bd/s//.B/
g/^\.it/s//.I/
g/^\.dt/s//.DT/
g/\\\*r/s//\\*R/
g/^\.lp/s/.*$/==>DOES NOT CONVERT EASILY - SEE .RS\/.RE\/.TP in MAN.7/
g/^\.s[123]/s/.*$/==>DOES NOT CONVERT EASILY - SEE .PD\/.P in MAN.7/
w temp.n
q
!
mv temp.n $1.n
shift
done
