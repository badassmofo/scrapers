#!/usr/bin/env python3
import os, sys, re

work = [(r'526172211a070(0|100)', ['rar', 'cbr']),
        (r'504b0[357]04', ['zip', 'cbz']),
        (r'4344303031', ['iso']),
        (r'89504e470d0a1a0a', ['png']),
        (r'ffd8ff', ['jpg', 'jpeg', 'jfif', 'jpe']),
        (r'474946383[79]61', ['gif']),
        (r'424d', ['bmp']),
        (r'(49(2049|492a00$)|4d4d002[ab])', ['tiff', 'tif']),
        (r'1a45dfa3(a3|9f)', ['mkv']),
        (r'1a45dfa301', ['webm']),
        (r'.{8}66747970(33677035|4d534e56|69736f6d|6d703432)', ['mp4']),
        (r'52494646.{8}415649204c495354', ['avi']),
        (r'000000146674797071742020', ['mov']),
        (r'464c5601', ['flv']),
        (r'.{8}667479706d703432', ['m4v']),
        (r'52494646.{8}57415645666d7420', ['wmv']),
        (r'494433', ['mp3']),
        (r'.{8}667479704d344120', ['m4a']),
        (r'4F67675300020{16}', ['ogg', 'oga', 'ogv', 'ogx']),
        (r'4d546864', ['midi', 'mid', 'pcs'])]

ignore = ['txt', 'py', 'c', 'cpp', 'rb', 'pl', 'xml', 'json']
home   = os.environ['HOME']

def safe_rename(a, b):
    if os.path.exists(b):
        b_a  = b.split('.')
        ext  = b_a[-1]
        path = '.'.join(b_a[:-1])
        inc  = 2

        while True:
            b = "{} ({}).{}".format(path, inc, ext)
            if not os.path.exists(b):
                os.rename(a, b)
                break
            inc += 1
    else:
        os.rename(a, b)
    print("Moving {} -> {}".format(a.replace(home, '~'), b.replace(home, '~')))

files, hide_ok, fix_broken = [], False, False
for a in sys.argv[1:]:
    if a.startswith('--'):
        a = a[2:]
        if a == 'fix':
            fix_broken = True
        elif a == 'broken':
            hide_ok = True
    else:
        files.append(a)

for a in files:
    with open(a, 'rb') as f:
        buf = ''.join(['{:02X}'.format(b) for b in f.read(16)]).lower()
    path   = a.replace(home, '~')
    path_a = a.split('.')
    ext    = path_a[-1]
    found  = False

    for e in work:
        if re.match(e[0], buf):
            if ext in e[1]:
                print(path + " is OK")
            else:
                print(path + " incorrect ext \"{}\" ->".format(ext))
                print("   it's actually \"{}\"".format(e[1][0]))
                if fix_broken:
                    safe_rename(a, "{}.{}".format('.'.join(path_a[:-1]), e[1][0]))
                found = True
            break

    if not found:
        print(path + " is unknown, sorry!")
