#!/bin/bash

# check for 2 arguments and that are positive numbers and the first is smaller than the second
if [ $# -ne 2 ] || [ $1 -lt 0 ] || [ $2 -lt 0 ] || [ $1 -gt $2 ]; then
	echo "Usage: $0 <start> <stop>"
	exit 1
fi

# go through the numbers from start to stop increasing by 2^i where i is the enumeration of the loop
for ((i= 0, j = $1; j <= $2; i++, j = $1 + 2**i)); do
	echo $j
done

echo $j