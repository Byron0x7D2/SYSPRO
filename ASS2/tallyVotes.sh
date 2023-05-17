#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: $0 <tallyResaultsFile>"
	exit 1
fi

if [ ! -f inputFile ]; then
	echo "inputFile does not exist in current directory"
	exit 1
fi

cat inputFile | awk '{print $2}' | sort | uniq -c | awk '{print $2, $1}' > $1
