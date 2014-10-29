#!/bin/sh

#   ---------------------------------------------
#   Set PATH
#   ---------------------------------------------

LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib
PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/usr/local/sbin:/opt/bin

get_model()
{
	CMDLINE=`cat /proc/cmdline`
	for x in $CMDLINE; do
		case $x in
			model=*)
				echo "${x#model=}"
				break;
				;;
		esac
	done
}

get_sn()
{
	CMDLINE=`cat /proc/cmdline`
	for x in $CMDLINE; do
		case $x in
			sn=*)
				echo "${x#sn=}"
				break;
				;;
		esac
	done
}

get_version()
{
	echo `cat /etc/version`
}

get_wan()
{
	WAN=`cat /etc/wan`
	if [ "$WAN" != "ppp0" ]
	then
		WAN="eth0"
	fi

	echo $WAN
}
