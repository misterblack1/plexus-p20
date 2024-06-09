# 'perform disk accounting'
PATH=:/bin:/usr/lib/acct/bin:/usr/bin:/etc
export PATH
dir=/usr/adm
pickup=acct/nite
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
cd ${dir}
date
find / -print | acctdusg >dtmp
date
sort +0n +1 -o dtmp dtmp
acctdisk <dtmp >disktmp
chmod 644 disktmp
chown adm disktmp
mv disktmp ${pickup}/disktacct
