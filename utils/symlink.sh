#!/usr/bin/env bash

cat symlink.json | jq -r 'keys[] as $k | $k + " " + .[$k]' | while read -r line; do
	last=`echo $line | grep -o '[^ ]*$'`
	args=`echo "$PWD/$line" | awk '{$NF=""; print $0}'`
	to="${last/#\~/$HOME}"
	if [[ "${to: -1}" == '/' ]]; then
		if [[ ! -d $to ]]; then
			if [[ -f `echo $line | cut -d " " -f1` ]]; then
				mkdir $to
			fi
		fi
	fi
	ln -s $args$to
done
