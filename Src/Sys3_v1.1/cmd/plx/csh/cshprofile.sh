setenv LOGNAME "$LOGNAME"
setenv TZ PST8PDT
cat /etc/motd
if { mail -e } then
	echo "you have mail"
endif
if ( $LOGNAME != root ) then
	news -n
endif
