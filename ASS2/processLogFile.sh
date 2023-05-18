#!/bin/bash

if [ $# -ne 1 ]; then # check if the number of arguments is correct
	echo "Usage: $0 <poll-log>"
	exit 1
fi

if [ ! -r $1 ]; then # check if the file exists and is readable
	echo "$1 does not exist in current directory or cannot be read"
	exit 1
fi

cat $1 > temp1  
awk '{print $2}' temp1 > temp2 # get the party names
sort temp2 > temp3 # sort the names
uniq -c temp3 > temp4 # count the names
awk '{print $2, $1}' temp4 > pollerResaultsFile # write the names and the number of votes in a file in the reverse order of temp 4
rm temp1 temp2 temp3 temp4 # remove the temporary files


