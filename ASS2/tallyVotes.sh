#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: $0 <tallyResaultsFile>"
	exit 1
fi

# check if inputFile.txt exists in current directory
if [ ! -f inputFile.txt ]; then
	echo "inputFile.txt does not exist in current directory"
	exit 1
fi

# from inputFile.txt, count the number of votes for each party, if there is a duplicate name, keep only the first one, then in the argument file, each line have the format: <partyName> <numVotes>
cat inputFile.txt | awk '{print $2}' | sort | uniq -c | awk '{print $2, $1}' > $1
