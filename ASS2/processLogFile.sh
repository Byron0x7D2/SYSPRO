#!/bin/bash

if [ $# -ne 1 ]; then # check if the number of arguments is correct
	echo "Usage: $0 <poll-log>"
	exit 1
fi

if [ ! -r $1 ]; then # check if the file exists and is readable
	echo "$1 does not exist in current directory or cannot be read"
	exit 1
fi

awk '{print $3}' $1 > temp1 # get the party names
sort temp1 > temp2 # sort the names
uniq -c temp2 > temp3 # count the names
awk '{print $2, $1}' temp3 > pollerResaultsFile # write the names and the number of votes in a file in the reverse order of temp 4
rm  temp1 temp2 temp3 # remove the temporary files


