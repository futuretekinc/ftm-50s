#!/bin/sh

if [ ! -e /etc/package_installed ] ; then
	if [ ! -e /etc/init.d/thingplus ] ; then
		cp package/thingplus /etc/init.d/thingplus
		cd /etc/rc.d
		ln -s ../init.d/thingplus S90thingplus
	fi	
fi
