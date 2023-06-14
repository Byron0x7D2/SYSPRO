#!/bin/bash


# check if the number of arguments is 1
if [ $# -ne 1 ]; then
	echo "Usage: $0 <dir>"
	exit 1
fi

# if it is a file, remove it
if [ -f $1 ]; then
	tar -cf $HOME/$(basename $1).tar $1
	rm $1
	exit 0
fi

# if it is a directory, recursively remove all files in it
if [ -d $1 ]; then
	files = ($("ls" -a $1 | awk '$0 != "." && $0 != ".."'))
	for file in "${files[@]}"; do
		./rem.sh $file
	done
	tar -cf $HOME/$(basename $1).tar $1
	rmdir $1
	exit 0
fi

