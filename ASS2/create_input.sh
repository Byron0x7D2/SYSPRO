#!/bin/bash

# possible characters
characters='abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'


if [ $# -ne 2 ]; then # check if the number of arguments is correct
	echo "Usage: $0 <political Parties> <numLines>"
	exit 1
fi

if [ -f inputFile.txt ]; then
	rm inputFile.txt
fi
touch inputFile.txt # create the file 

for (( i=0; i<$2; i++ )); do
	length=$(( RANDOM % 10 + 3 )) # random length of the name

	randomString=""
	for (( j=0; j<$length; j++ )); do
		randomString=$randomString${characters:$(( RANDOM % ${#characters} )):1}
	done

	length=$(( RANDOM % 10 + 3 )) # random length of the name
	
	randomString2=""
	for (( j=0; j<$length; j++ )); do
		randomString2=$randomString2${characters:$(( RANDOM % ${#characters} )):1}
	done

	randomParty=$(shuf -n 1 $1) # choose a random party from the file

	echo "$randomString $randomString2 $randomParty" >> inputFile.txt # write the name and the party in the file
done


