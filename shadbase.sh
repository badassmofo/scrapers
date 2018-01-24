#!/usr/bin/env sh

function ABANDONSHIP {
	rm index.html > /dev/null
	rm page.html > /dev/null
	exit 0
}

p=1
while true; do
	wget -q http://www.shadbase.com/category/archiveall/page/$p/ -O index.html
	if [ $? -ne 0 ]; then
		ABANDONSHIP
	fi

	cat index.html | grep -oP "<a href=\"http:\/\/www\.shadbase\.com\/(?!feed|category|archive|about|forum|non-porn-section)[a-z0-9-]+\/" | cut -c 10- | while read -r l; do
		wget -q $l -O page.html
		if [ $? -ne 0 ]; then
			ABANDONSHIP
		fi

		cat page.html | grep -oP "http:\/\/www\.shadbase\.com\/comic_folder\/[a-z0-9-]+\.jpg" | while read -r i; do
			wget -nc -q $i

			if [ $? -ne 0 ]; then
				ABANDONSHIP
			fi
		done
	done

	p=$((p + 1))
done
