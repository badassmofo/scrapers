#!/usr/bin/env python3
import re, json, os

def standard(cmd):
    return os.popen(cmd).read().split('\n')[:-1]

def remove_versions(arr):
    return [a.split(' ')[0] for a in arr]

def gems():
    return remove_versions(standard("gem list"))

def pips():
    return remove_versions(standard("pip3 list --format=legacy"))

def node():
    return [n.split('@')[0].split(' ')[1] for n in standard("npm list -g --depth=0") if '@' in n]

def apm():
    y = list(reversed(standard("apm list")))
    return [x.split(' ')[1].split('@')[0] for x in y[:len(y) - int(y[-1][:-1].split('(')[1]) - 3] if re.match(r'.* \S+@\d+\.\d+\.\d+', x)]

print(json.dumps({"brew install": standard("brew leaves"), "brew cask install": standard("brew cask list"), "gem install": gems(), "pip3 install": pips(), "npm install": node()}, sort_keys=True, indent=2))
