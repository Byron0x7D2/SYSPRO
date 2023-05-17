#!/bin/bash


if [ $# -ne 2 ]; then
	echo "Usage: $0 <political Parties> <numLines>"
	exit 1
fi

touch inputFile.txt

for (( i=0; i<$2; i++ )); do
	randomString=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $(( RANDOM % 10 + 3 )) | head -n 1)
	randomParty=$(sed -n "$(( RANDOM % $(wc -l < $1) + 1 ))p" $1)
	echo "$randomString $randomParty" >> inputFile.txt
done


