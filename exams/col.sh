#!/bin/bash

# check if the file exists
if [ ! -f $1 ]; then   # use -e for existence
	echo "File does not exist"
	exit 1
fi

# find max of the first column
echo $(awk '{print $1}' $1 | sort -n | tail -1)


# find min of the second column
echo $(awk '{print $2}' $1 | sort -n | head -1)


# find sum of the third column
echo $(awk '{print $3}' $1 | awk '{sum+=$1} END {print sum}')

# find median of the fourth column
echo $(awk '{print $4}' $1 | sort -n | awk '{a[i++]=$1} END {print a[int(i/2)]}')
median=$(echo "$column4" | sort -n | awk 'NR == (FNR + 1) / 2 {print}')
echo median of 4rd column = `awk '{print $4}' $file | awk '{sum+=$1 ; i+=1} END {print sum/i}'`