#       "nitely accounting shell, should be run from cron (adm) at 4am"
#	"does process, connect, disk, and fee accounting"
#	"prepares command summaries"
#	"shell is restartable and provides reasonable diagnostics"
a=/usr/adm
b=acct/nite
c=$a/$b
e=acct/sum
export PATH
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
statefile=${c}/statefile
active=${c}/active
lastdate=${c}/lastdate
errormsg="\n\n\n\n************ ACCT ERRORS --- RUN ABORTED : see  ${active}${d} ************\n\n\n\n"
cd $a
#		"make sure that 2 crons weren't started, or leftover problems"
date  > $b/lock1
chmod 400 $b/lock1
ln $b/lock1 $b/lock
if test $? -ne 0; then
	l="\n\n\n\n*************** 2 CRONS or ACCT PROBLEMS ***************\n\n\n\n"
	(date ; echo "$l" ) >/dev/console
	echo "$l" | mail adm root
	echo "ERROR: locks found, run aborted" >> ${active}
	rm -f ${c}/lock*
	exit 1
fi


case $# in
0)
#	"as called by the cron each day"
	d="`date +%m%d`"
	if test ! -r ${lastdate} ; then
		echo "0000" > ${lastdate}
	fi
	if test "${d}" = "`cat ${lastdate}`"; then
		(date; echo "${errormsg}") > /dev/console
		echo "${errormsg}" | mail root adm
		echo "ERROR: acctg already run for `date`: check ${lastdate}" >> ${active}
		rm -f ${c}/lock*
		mv ${active} ${active}${d}
		exit 1
	fi
	echo ${d} > ${lastdate}
	echo "SETUP" > ${statefile}
	nulladm ${active}
	echo "\n\n\n\n\n**********  SYSTEM ACCOUNTING STARTED `date`  **********\n\n\n\n\n" > /dev/console
	;;

1)
#	"runacct MMDD  (date)  will restart at current state"
	d=$1
	echo "restarting acctg for ${d} at `cat ${statefile}`" >> ${active}
	echo "\n\n\n\n\n********** SYSTEM ACCOUNTING RESTARTED `date` **********\n\n\n\n\n" > /dev/console
	;;

2)
#	"runacct MMDD STATE  restart at specified state"
	d=$1
	echo "restarting acctg for ${d} at $2" >> ${active}
	echo "previous state was `cat ${statefile}`" >> ${active}
	echo "$2" > ${statefile}
	echo "\n\n\n\n\n********** SYSTEM ACCOUNTING RESTARTED `date` **********\n\n\n\n\n" > /dev/console
	;;
*)
	(date; echo "${errormsg}") > /dev/console
	echo "${errormsg}" | mail root adm
	echo "ERROR: runacct called with invalid arguments" > ${active}
	rm -f ${c}/lock*
	mv ${active} ${active}${d}
	exit 1
	;;
esac


#	"processing is broken down into seperate, restartable states"
#	"the statefile is updated at the end of each state so that the"
#	"next loop through the while statement switches to the next state"
while [ 1 ]
do
case "`cat ${statefile}`" in
SETUP)

cd $a
(date ; ls -l fee pacct* wtmp* ) >> ${active}

#	"switch current pacct file"
turnacct switch
rc=$?
if test ${rc} -ne 0; then
	(date ; echo "${errormsg}" ) > /dev/console
	echo "${errormsg}" | mail root adm
	echo "ERROR: turnacct switch returned rc=${rc}" >> ${active}
	rm -f ${c}/lock*
	mv ${active} ${active}${d}
	exit 1
fi

#	" give pacct files unique names for easy restart "
for i in pacct?
do
	if test -r S${i}.${d} ; then
		 (date ; echo "${errormsg}" ) > /dev/console
		echo "${errormsg}" | mail root adm
		echo "ERROR: S${i}.${d} already exists" >> ${active}
		echo "file setups probably already run" >> ${active}
		rm -f ${c}/lock*
		mv ${active} ${active}${d}
		exit 1
	fi
	mv ${i} S${i}.${d}
done


#	"add current time on end"
if test -r ${b}/wtmp.${d} ; then
	(date ; echo "${errormsg}" ) > /dev/console
	echo "${errormsg}" | mail root adm
	echo "ERROR: ${b}/wtmp.${d} already exists: run setup manually" > ${active}
	rm -f ${c}/lock*
	mv ${active} ${active}${d}
	exit 1
fi
cp wtmp ${b}/wtmp.${d}
acctwtmp "" "console" >> ${b}/wtmp.${d}
nulladm wtmp


#	"copy pacct and wtmp files for backup copies"
#	" to be removed after a filesave"
#	"these lines should be commented out if your /usr space is low"
cp ${b}/wtmp.${d} ${e}/wtmp.${d}
cat Spacct?.${d} > ${e}/pacct.${d}



echo "files setups complete" >> ${active}
echo "WTMPFIX" > ${statefile}
;;


WTMPFIX)
#	"verify the integrity of the wtmp file"
#	"wtmpfix will automatically fix date changes"
cd $c
nulladm tmpwtmp wtmperror
wtmpfix < wtmp.${d} > tmpwtmp 2>wtmperror
if test $? -ne 0 ; then
	(date ; echo "${errormsg}") | mail root adm
	echo "${errormsg}" > /dev/console
	echo "ERROR: wtmpfix errors see ${c}/wtmperror${d}" >> ${active}
	rm -f ${c}/lock*
	mv ${active} ${active}${d}
	mv wtmperror wtmperror${d}
	exit 1
fi

echo "wtmp processing complete" >> ${active}
echo "CONNECT1" > ${statefile}
;;


CONNECT1)
#	"produce connect records in the ctmp.h format"
#	"the lineuse and reboots files are used by prdaily"
cd $c
nulladm lineuse reboots ctmp log
acctcon1 -t -l lineuse -o reboots < tmpwtmp  2> log |\
sort +1n +2 > ctmp 
if test -s log ; then
	(date ; echo "${errormsg}") | mail adm root
	echo "${errormsg}" > /dev/console
	echo "ERROR: connect acctg failed:  check ${c}/log" >> ${active}
	rm -f ${c}/lock*
	mv ${active} ${active}${d}
	mv ${c}/log ${c}/log${d}
	exit 1
fi

echo "CONNECT2" > ${statefile}
;;


CONNECT2)
#	"convert ctmp.h records in tacct records"
cd $c
nulladm ctacct.${d}
acctcon2 < ctmp | acctmerg > ctacct.${d}

echo "connect acctg complete" >> ${active}
echo "PROCESS" > ${statefile}
;;


PROCESS)
#	"correlate Spacct and ptacct files by number"
#	"will not process Spacct file if corresponding ptacct exists"
#	"remove the ptacct file to rurun the Spacct file"
#	"if death occurs here, rerunacct should remove last ptacct file"
cd $c
num=""
for i in 1 2 3 4 5 6 7 8 9
do
	if test -r ${a}/Spacct${i}.${d}; then
		num="${num} ${i}"
	fi
done
for i in ${num}
do
	if test -s ptacct${i}.${d}; then
		echo "WARNING: process acctg already run for Spacct${i}.${d}" >> ${active}
		echo "WARNING: remove ${c}/ptacct${i}.${d} to rerun" >> ${active}
	else
		nulladm ptacct${i}.${d}
                acctprc1 ctmp < ${a}/Spacct${i}.${d} |\
		acctprc2 > ptacct${i}.${d}
		echo "process acctg complete for Spacct${i}.${d}" >> ${active}
	fi
done
echo "all process actg complete for ${d}" >> ${active}
echo "MERGE" > ${statefile}
;;


MERGE)
cd $c
#	"merge ctacct and ptacct files together"
acctmerg ptacct?.${d} < ctacct.${d} > daytacct

echo "tacct merge to create daytacct complete" >> ${active}
echo "FEES" > ${statefile}
;;


FEES)
cd $c
#	"merge in fees"
if test -s $a/fee; then
	cp daytacct tmpdayt
	sort +0n +2 $a/fee | acctmerg -i | acctmerg tmpdayt  > daytacct
	echo "merged fees" >> ${active}
	rm -f tmpdayt
else
	echo "no fees" >> ${active}
fi
echo "DISK" > ${statefile}
;;


DISK)
cd $c
#	"the last act of any disk acct procedure should be to mv its"
#	"entire output file to disktacct, where it will be picked up"
if test -r disktacct; then
	cp daytacct tmpdayt
	acctmerg disktacct  < tmpdayt > daytacct
	echo "merged disk records" >> ${active}
	rm -f tmpdayt disktacct
else
	echo "no disk records" >> ${active}
fi
echo "MERGETACCT" > ${statefile}
;;

MERGETACCT)
cd ${a}/acct
#	"save each days tacct file in sum/tacct.${d}"
#	"if sum/tacct gets corrupted or lost, could recreate easily"
#	"the mounthly acctg procedure should remove all sum/tacct files"
cp nite/daytacct sum/tacct$d
if test ! -r sum/tacct; then
	echo "WARNING: recreating ${a}/sum/tacct " >> ${active}
	nulladm sum/tacct
fi

#	"merge in todays tacct with the summary tacct"
cp sum/tacct sum/tacctprev
acctmerg sum/tacctprev  < sum/tacct$d > sum/tacct

echo "updated sum/tacct" >> ${active}
echo "CMS" > ${statefile}
;;


CMS)
cd ${a}/acct
#	"do command summaries"
nulladm sum/daycms
if test ! -r sum/cms; then
	nulladm sum/cms
	echo "WARNING: recreating ${a}/sum/cms " >> ${active}
fi
cp sum/cms sum/cmsprev
acctcms $a/Spacct?.${d}  > sum/daycms
acctcms -s sum/daycms sum/cmsprev  > sum/cms
acctcms -a -s sum/daycms | sed -n 1,56p  > nite/daycms
acctcms -a -s sum/cms | sed -n 1,56p  > nite/cms
lastlogin 
echo "command summaries complete" >> ${active}
echo "USEREXIT" > ${statefile}
;;


USEREXIT)
#	"any installation dependant accounting programs should be run here"


echo "CLEANUP" > ${statefile}
;;


CLEANUP)
cd ${a}/acct
#	" finally clear files; could be done next morning if desired"
nulladm $a/fee
rm -f ${a}/Spacct?.${d}
#	"put reports onto a file"
prdaily >> sum/rprt$d;
rm -f nite/lock*
rm -f nite/ptacct?.${d} nite/ctacct.${d}
rm -f nite/wtmp.${d} nite/wtmperror${d} nite/active${d}
mv nite/tmpwtmp nite/owtmp
echo "system accounting completed at `date`" >> ${active}
echo "********** SYSTEM ACCOUNTING COMPLETED `date` **********" > /dev/console
echo "COMPLETE" > ${statefile}
exit 0
;;

*)
	(date;echo "${errormsg}") > /dev/console
	echo "${errormsg}" | mail adm root
	echo "ERROR: invalid state, check ${statefile}" >> active
	rm -f ${c}/lock*
	mv ${active} ${active}${d}
	exit 1
	;;
esac
done


#	" runacct is normally called with no arguments from the cron"
#	" it checks its own locks to make sure that 2 crons or previous"
#	" problems have not occured"

#	" runacct uses the statefile to record its progress"
#	" each state updates the statefile upon completion"
#	" then the next loop though the while picks up the new state"

#	" to restart this shell,  check the active file for diagnostics"
#	" fix up any corrupted data (ie. bad pacct or wtmp files)"
#	" if runacct detected the error it removes the locks"
#	" remove the locks if necessary, otherwise runacct will complain"
#	" the lastdate file should be removed or changed"
#	" restart runacct at current state with:  runacct MMDD"
#	" to override the statefile: runacct MMDD STATE"


#	" if runacct has been executed after the latest failure"
#	" ie. it ran ok today but failed yesterday"
#	" the statefile will not be correct"
#	" check the active files and restart properly"

#	" if runacct failed in the PROCESS state, remove the last"
#	" ptacct file because it may not be complete"

#	" if shell has failed several days, do SETUP manually"
#	" then rerun runacct once for each day failed"
#	" could use fwtmp here to split up wtmp file correctly"

#	" normally not a good idea to restart the SETUP state"
#	" should be done manually, or just cleanup first"


#	" FILE USAGE:	all files in /usr/adm/ac/nite unless specified"

#	" statefile	records progess of runacct"
#	" lastdate	last day runacct ran in date +%m%d format"
#	" lock lock1	controls serial use of runacct"
#	" active	place for all descriptive and error messages"
#	" fd2log	fd2 output for runacct ( see cron entry ) "
#	" wtmp.MMDD owtmp yesterdays wtmp file"
#	" tmpwtmp	yesterdays wtmp corrected by wtmpfix"
#	" wtmperror	place for wtmpfix error messages"
#	" lineuse	lineusage report used in prdaily"
#	" reboots	reboots report used in prdaily"
#	" ctmp	ctmp.h records from acctcon1"
#	" log		place for error messages from acctcon1"
#	" ctacct.MMDD	connect tacct records for MMDD"
#	" ptacct.n.MMDD	process tacct records n files for MMDD"
#	" daytacct	total tacct records for this days accounting"
#	" disktacct	disk tacct records produced by disk shell"
#	" daycms	ascii daily command summary used by prdaily"
#	" cms		acsii total command summary used by prdaily"

#	" following files in /usr/adm directory"

#	" fee		output from chargefee program"
#	" pacct		active pacct file"
#	" pacctn	switched pacct files"
#	" Spacctn.MMDD	pacct files for MMDD after SETUP state"
#	" wtmp		active wtmp file"

#	" following files in /usr/adm/acct/sum"

#	" loginlog	output of lastlogin used in prdaily"
#	" tacct		total tacct file for current fiscal"
#	" tacct.MMDD	tacct file for day MMDD"
#	" cms		total cms file for current fiscal"
#	" rprt.MMDD	output of prdaily program"
#	" wtmp.MMDD	saved copy of wtmp for MMDD"
#	" pacct.MMDD	concatenated version of all pacct files for MMDD"
#	" cmsprev	total cms file without latest update"
#	" tacctprev	total tacct file without latest update"
#	" daycms	cms files for todays usage"
