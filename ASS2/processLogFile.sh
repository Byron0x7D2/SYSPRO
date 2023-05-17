#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: $0 <poll-log>"
	exit 1
fi

if [ ! -r $1 ]; then
	echo "$1 does not exist in current directory or cannot be read"
	exit 1
fi

cat $1 > temp1 
awk '{print $2}' temp1 > temp2
sort temp2 > temp3
uniq -c temp3 > temp4
awk '{print $2, $1}' temp4 > pollerResaultsFile
rm temp1 temp2 temp3 temp4


