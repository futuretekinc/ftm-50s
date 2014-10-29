#!/bin/sh

WAN=`cat /etc/wan`
case '$IFACE' in
	'lo')
		echo -n > /etc/interfaces
		echo 'auto $IFACE' >> /etc/interfaces
		echo 'iface $IFACE inet loopback' >> /etc/interfaces
		;;

	'eth0')
		if [ '$WAN' = 'eth0' ]
		then
			echo -n > /etc/interfaces
			echo 'auto $IFACE' >> /etc/interfaces
			echo 'iface $IFACE inet dhcp' >> /etc/interfaces
		else
			ifconfig $IFACE 0.0.0.0
	
		fi
		;;

	'eth1')
		if [ '$WAN' = 'eth0' ]
		then
			echo -n > /etc/interfaces
			cat /etc/configs/eth1 >> /etc/interfaces
		else
			ifconfig $IFACE 0.0.0.0
	
		fi
		;;

esac


