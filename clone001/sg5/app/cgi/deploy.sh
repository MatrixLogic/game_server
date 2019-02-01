#/bin/sh

dir=`dirname $0`
for cgi in `ls "$dir" | grep "Cgi[^\.]*$"`
	do cp "$dir/$cgi" /data/rein/cgi/ -f
	echo "$dir/$cgi"
	done
#cd /data/release/sgonline/cgi
#./deploy.sh
#cd /data/release/sgonline/lighttpd/sbin
#./restart.sh
