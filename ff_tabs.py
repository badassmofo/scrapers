#!/usr/bin/env python3
import lz4.block, json

for window_idx, window in enumerate(json.loads(lz4.block.decompress(open('/Users/roryb/Library/Application Support/Firefox/Profiles/8udnevh5.default/sessionstore-backups/recovery.jsonlz4', 'rb').read().replace(b'mozLz40\x00', b'', 1)))['windows']):
    for tab in window['tabs']:
        print(tab['entries'][-1]['url'])
