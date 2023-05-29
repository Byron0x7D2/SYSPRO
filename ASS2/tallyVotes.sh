#!/bin/bash
# almost same as processLogFile.sh so same comments :), except the 2 new lines
if [ $# -ne 1 ]; then
	echo "Usage: $0 <tallyResaultsFile>"
	exit 1
fi

if [ ! -f inputFile ]; then
	echo "inputFile does not exist in current directory"
	exit 1
fi

sed '/^$/d' inputFile > temp4 # removes empty lines
awk '!seen[$1,$2]++{print $1, $2, $3}' temp4 > temp0 # removes double entries
awk '{print $3}' temp0 > temp1
sort temp1 > temp2
uniq -c temp2 > temp3
awk '{print $2, $1}' temp3 > $1
rm temp1 temp2 temp3 temp0 temp4
 