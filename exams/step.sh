#!/bin/bash

# take 3 arguments, check the second parameter is positive and the first parameter is smaller than the third parameter
if [ $# -ne 3 ] || [ $2 -le 0 ] || [ $1 -ge $3 ]; then
	echo "Usage: $0 <start> <step> <end>"
	exit 1
fi

# print the numbers from start to end with step
for (( i=$1; i<=$3; i+=$2 )); do
	echo $i
done