#	"nulladm name..."
#	"creates each named file mode 664"
#	"make sure owned by adm (in case created by root)"
for i
do
	cp /dev/null $i
	chmod 664 $i
	chown adm $i
	chgrp adm $i
done
