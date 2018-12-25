#!/bin/sh
for EXTURL in "$@"
do
	open -a /Applications/Chromium.app "https://clients2.google.com/service/update2/crx?response=redirect&prodversion=47.0&x=id%3D$(echo $EXTURL | rev | cut -d '/' -f 1 | rev | cut -d '?' -f 1)%26installsource%3Dondemand%26uc"
done
