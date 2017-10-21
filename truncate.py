#!/usr/bin/env python3
import sys, threading

last_line, same = None, 2
try:
    for line in sys.stdin:
        if not line == last_line:
            sys.stdout.write(line)
            last_line = line
        else:
            print(u'\u001B[1A' + '{} x{}'.format(line[:-1], same))
            same += 1
except KeyboardInterrupt:
    sys.exit(0)
