#!/bin/sh

time=0
while [ 0 ]
do
   time=`expr $time + 1`
   if [ $time -eq 200 ]
   then
       break
   fi
done

file="/var/log/modem"
if [ -f $file ]
then
	raw=`cat /var/log/modem | sed /^$/d | awk '/AT\+CGEQREQ\?/{ print NR }' | awk 'END { print }'`
	if [ -n "$raw" ]
	then
		next_raw=`expr $raw + 2`
		result=`cat /var/log/modem | sed /^$/d | sed -n "$next_raw"p | awk '{ print $1 }'`
		#echo $result
		if [ "$result" = "+CGEQREQ:" ]
		then
			cmd=`cat /var/log/modem | sed /^$/d | sed -n "$next_raw"p'`
			echo $cmd
		elif [ "$result" = "+CME" ]
		then
			cmd=`cat /var/log/modem | sed /^$/d | sed -n "$next_raw"p'`
			echo $cmd
		else 
			echo "URC MESSAGE"
		fi
	else
		echo done
	fi
else
	echo done
fi
