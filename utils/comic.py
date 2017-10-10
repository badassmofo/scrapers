#!/usr/bin/env python3
import os, sys, re

exts    = ['rar', 'zip', 'cbr', 'cbz']
exts_d  =  dict(zip(exts[:2], exts[2:]))
exts_re = "({})".format('|'.join(exts))
file_re = re.compile(r"^(.*)\.{}$".format(exts_re))
home    = os.environ['HOME']

for a in sys.argv[1:]:
    if os.path.exists(a):
        m = file_re.match(a)
        p = a.replace(home, '~')
        if m:
            to = "{}.{}".format(m.group(1), exts_d[m.group(2)])
            try:
                os.rename(a, to)
                print("{} -> {}".format(p, to.replace(home, '~')))
            except:
                print("Failed to rename \"{}\"".format(p))
        else:
            print("Ignoring \"{}\"".format(p))
    else:
        print("Ignoring \"{}\"".format(a))
