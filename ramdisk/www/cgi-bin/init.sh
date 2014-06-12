#!/bin/sh
mmd -p /dev/ttyS1 -c at+cgatt=0
sleep 1
cp /etc/initQos.conf /etc.init/
rm -rf /mnt/mtd/etc
sleep 1
cp -r /etc.init /mnt/mtd/etc
sleep 2
sync
reboot
