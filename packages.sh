#!/usr/bin/env sh

cat packages.json | jq -r 'keys[] as $k | .[$k] | map(.+" ") | join(" ") as $v | $k + " " + $v' | while read -r line; do
  eval $line
done
