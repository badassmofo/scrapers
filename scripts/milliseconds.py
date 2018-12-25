#!/usr/bin/env python3
import sys, re

if len(sys.argv) <= 1:
	print(0)
	sys.exit(-1)

if re.match('^[0-9]+$', sys.argv[1]):
	time = int(sys.argv[1])
	if len(sys.argv) > 2:
		noun = sys.argv[2]
		if re.match('^sec(s|onds?)$', noun):
			print(time * 1000)
		elif re.match('^min(s|utes?)$', noun):
			print(time * 60 * 1000)
		elif re.match('^hours?$', noun):
			print(time * 60 * 60 * 1000)
	else:
		print(time * 1000)
elif re.match('^((1[0-9]|2[0-4]):)?([1-5]?[0-9]):([1-5]?[0-9])$', sys.argv[1]):
	time   = [int(t) for t in sys.argv[1].split(':')[::-1]]
	m      = time[0] * 1000 + time[1] * 60 * 1000
	if len(time) == 3:
		m += time[2] * 60 * 60 * 1000
	print(m)
else:
	print(0)