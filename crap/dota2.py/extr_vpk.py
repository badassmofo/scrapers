import vpk
from sys import argv
from os.path import expanduser
import platform

p = platform.system()
print vpk.open("%s/Steam/steamapps/common/dota 2 beta/game/dota/pak01_dir.vpk" % (expanduser('~/Library/Application Support') if p == 'Darwin' else 'C:/Program Files (x86)' if p == 'Windows' else expanduser('~/.steam')))[argv[1]].read()
