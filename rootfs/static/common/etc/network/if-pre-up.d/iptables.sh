#!/bin/sh

cat /etc/sysconfig/iptables

/sbin/iptables-restore < /etc/sysconfig/iptables

