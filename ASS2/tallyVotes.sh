#!/bin/bash
# almost same as processLogFile.sh so same comments :)
if [ $# -ne 1 ]; then
	echo "Usage: $0 <tallyResaultsFile>"
	exit 1
fi

if [ ! -f inputFile ]; then
	echo "inputFile does not exist in current directory"
	exit 1
fi


cat inputFile > temp1
awk '{print $2}' temp1 > temp2
sort temp2 > temp3
uniq -c temp3 > temp4
awk '{print $2, $1}' temp4 > $1
rm temp1 temp2 temp3 temp4
 