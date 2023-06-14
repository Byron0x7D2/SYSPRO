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


# fib
fn_1=1
fn_2=1
for ((i=1; i <= "$1"; i++)) do
    echo "$fn_1" >> $2
    fn=`expr $fn_1 + $fn_2`
    fn_1=$fn_2
    fn_2=$fn
done

# fact
fact=1
for ((i=1; i <= "$1"; i++)) do
    fact=`expr $fact \* $i`
done
ech $fact

