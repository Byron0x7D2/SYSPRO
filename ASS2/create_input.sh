#!/bin/bash

characters='abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
length=$(( RANDOM % 10 + 3 ))

if [ $# -ne 2 ]; then
	echo "Usage: $0 <political Parties> <numLines>"
	exit 1
fi

touch inputFile

for (( i=0; i<$2; i++ )); do

	randomString=""
	for (( j=0; j<$length; j++ )); do
		randomString=$randomString${characters:$(( RANDOM % ${#characters} )):1}
	done

	randomParty=$(shuf -n 1 $1)

	echo "$randomString $randomParty" >> inputFile
done


