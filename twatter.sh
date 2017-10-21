#!/bin/sh
for d in $*; do
	for f in $d/*.jpg-large; do
		to=$(echo $f | sed 's/\.jpg-large/.jpg/')
		mv "$f" "$to"
		echo "$f -> $to"
	done
done
