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


awk '{print $2}' inputFile > temp1
sort temp1 > temp2
uniq -c temp2 > temp3
awk '{print $2, $1}' temp3 > $1
rm temp1 temp2 temp3 
 