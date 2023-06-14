#!/bin/bash

# check for 2 arguments, the first one be a number, second a file
if [ $# -ne 2 ] ; then
	echo "Usage: $0 <number> <file>"
	exit 1
fi

# if the second argument exists, delete it and create a new one
if [ -f $2 ] ; then
	rm $2
fi
touch $2

fn_1=1
fn_2=1
for ((i=1; i <= "$1"; i++)) do
    echo "$fn_1" >> $2
    fn=`expr $fn_1 + $fn_2`
    fn_1=$fn_2
    fn_2=$fn
done


# # from 1 to the first argument find the fibonacci number and print it to the file
# for (( i=1; i<=$1; i++ )); do
# 	if [ $i -eq 1 ] || [ $i -eq 2 ] ; then
# 		echo 1 >> $2
# 	else
# 		# get the last two numbers from the file
# 		last1=$(tail -n 1 $2)
# 		last2=$(tail -n 2 $2 | head -n 1)
# 		# add them together and print it to the file
# 		echo $((last1+last2)) >> $2
# 	fi
# done

