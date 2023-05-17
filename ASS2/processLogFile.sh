#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: $0 <poll-log>"
	exit 1
fi

# check if poll-log exists in current directory and can be read
if [ ! -r $1 ]; then
	echo "$1 does not exist in current directory or cannot be read"
	exit 1
fi

# poll log is of format <voterName> <partyName>
# from poll-log, count the number of votes for each party, if there is a duplicate name, keep only the first one, then in the pollerResaultsFile.sh, each line have the format: <partyName> <numVotes>

cat $1 | awk '{print $2}' | sort | uniq -c | awk '{print $2, $1}' > pollerResaultsFile