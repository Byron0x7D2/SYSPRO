#!/bin/bash

# check that 3 arguments are given 
if [ $# -ne 3 ]; then
	exit 1
fi

# check all 3 are positives and the first is smaller than the last
if [ $1 -lt 0 ] || [ $2 -lt 0 ] || [ $3 -lt 0 ] || [ $1 -gt $3 ]; then
	exit 1
fi

# set the start and the end of the search
start=$1
end=$3

# set the middle of the search
middle=$((($start + $end) / 2))
echo $middle

# while the middle is not the second argument

while [ $middle -ne $2 ]; do
	# if the middle is smaller than the second argument
	if [ $middle -lt $2 ]; then
		# set the start to the middle
		start=$middle
	# else
	else
		# set the end to the middle
		end=$middle
	fi
	# set the middle to the middle of the new search
	middle=$((($start + $end) / 2))
	echo $middle
done