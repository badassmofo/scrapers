#!/usr/bin/env sh

if [[ $1 != "heroes" && $1 != "items" ]]; then
    echo "invalid arguments"
    exit 1
fi

while read in; do
    cat data.json | jq -r ".$1[$in]"
done
