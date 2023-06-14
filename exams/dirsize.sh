#!/bin/bash 
max = 0; maxdir = $1; dirs = $*;
for dir in $dirs; do
	if [ -d $dir ]; then
		size = $(du -s $dir | awk '{print $1}')
		if [ $size -gt $max ]; then
			max = $size
			maxdir = $dir
		fi
	fi
done