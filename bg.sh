#!/usr/bin/env bash

convert \( -size `system_profiler SPDisplaysDataType | grep -Eohm1 "[0-9]{4} x [0-9]{4}" | tr -d [:space:]` xc:'rgb(35, 35, 35)' \) \( "original.jpg" -fuzz 10% -transparent white -colorspace gray +level-colors white,'rgb(35, 35, 35)' -trim \) -compose atop -gravity southeast -geometry +300 -composite "/Users/`whoami`/Pictures/bg_`openssl rand -hex 6`.png"
