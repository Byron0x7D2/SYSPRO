#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: $0 <poll-log>"
	exit 1
fi

if [ ! -r $1 ]; then
	echo "$1 does not exist in current directory or cannot be read"
	exit 1
fi

cat $1 | awk '{print $2}' | sort | uniq -c | awk '{print $2, $1}' > pollerResaultsFile