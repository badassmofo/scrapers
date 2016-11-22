#!/usr/bin/env bash

cat symlink.json | jq -r 'keys[] as $k | $k + " " + .[$k]' | while read -r line; do
	echo "ln -s $line"
done
