#!/usr/bin/env python3
# Idea stolen from http://pastebin.com/Zx0paABU
import re, requests, json, queue, threading
from functools import reduce

key                   = open('steam_key', 'r').read()  # Edit This
data                  = json.loads(open('data.json').read())
path_to_server_log    = ""  # Edit This
get_match_history_url = "http://api.steampowered.com/IDOTA2Match_570/GetMatchHistory/v001/?key={}&matches_requested=10&account_id=".format(key)
get_match_details_url = "http://api.steampowered.com/IDOTA2Match_570/GetMatchDetails/V001/?key={}&match_id=".format(key)
get_player_name_url   = "http://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key={}&steamids=".format(key)
means                 = ['kills', 'deaths', 'assists', 'last_hits', 'denies', 'gold_per_min', 'xp_per_min', 'level', 'hero_damage', 'tower_damage', 'hero_healing']
non_means             = ['last_10', 'abandons', 'hero', 'all_heroes', 'items']
steam_id64_base       = 76561197960265728
name_format           = "\033[2;37;40m{}\033[0;37;40m {} = {}"
user_agent_header     = {'user-agent': 'Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.1) Gecko/2008071615 Fedora/3.0.1-1.fc9 Firefox/3.0.1'}

def mean(arr):
    return reduce(lambda x, y: x + y, arr) / len(arr) if len(arr) > 0 else 0.0  # Don't divide by 0

def mode(x):
    a = max([x.count(y) for y in x])
    return list(set([z for z in x if x.count(z) == a])) if a > 1 else []

def get_match_history(pid):
    x = requests.get(get_match_history_url + pid).json()['result']
    return [str(mid['match_id']) for mid in x['matches']] if x['status'] == 1 else []  # Private profile

def get_match_details(mid, pid):
    z = requests.get(get_match_details_url + mid).json()['result']
    if 'error' in z:
        return None  # Probably a practise/bot match
    x = [y for y in z['players'] if y['account_id'] == pid][0]
    return (x, ((x['player_slot'] < 5 and z['radiant_win']) or (x['player_slot'] > 5 and not z['radiant_win']))) if len(x) else None

def get_player_history(pid):
    return [get_match_details(x, int(pid)) for x in get_match_history(pid)]

def get_player_name(pids):
    return {str(int(x['steamid']) - steam_id64_base): x['personaname'] for x in requests.get(get_player_name_url + ','.join([str(int(i) + steam_id64_base) for i in pids])).json()['response']['players']}

def get_dotabuff_info(pid):
    html = requests.get("http://www.dotabuff.com/players/" + pid, headers=user_agent_header).text
    awl = re.findall(r'<span class="(abandons|wins|losses)">(\d+,?\d+)<\/span>', html)
    return "No Dotabuff" if not len(awl) else "{} - {}".format('-'.join([x[1] for x in awl]), ', '.join([x[0] for x in re.findall(r'<dd>(\d+(\.\d+%)?)<\/dd>', html)]))

def get_player_summary(pid):
    hist = get_player_history(pid)
    if not hist:
        return {}  # Private Profile
    x, a = zip(*hist)
    wins = len([b for b in a if b])
    return {**{y: mean([z[y] for z in x]) for y in means}, **{'items': ', '.join([data['items'][c] for c in mode([a for b in [[z[y] for z in x] for y in ['item_' + str(i) for i in range(0, 6)]] for a in b])]), 'hero': ', '.join([data['heroes'][z] for z in mode([z['hero_id'] for z in x])]), 'all_heroes': ', '.join(list(set([data['heroes'][z['hero_id']] for z in x]))), 'abandons': len([z for z in x if z['leaver_status']]), 'wins': wins, 'last_10': "{}-{}".format(wins, 10 - wins)}}

def title(s):
    return ' '.join([x if x == 'per' else x[0].upper() + x[1:] for x in s.split('_')])

for line in reversed(list(open(path_to_server_log + "server_log.txt"))):
    if re.match(r'^\d+\/\d+\/\d+ - \d+:\d+:\d+: (loopback|=\[A:\d:\d+:\d+\]) \(Lobby \d+ DOTA.\S+ ([0-9]:\[U:1:\d+\]\s?){10}\).*?$', line):
        ids = [x.split(':')[-1][:-1] for x in re.findall(r'([0-9]:\[U:1:\d+\])', line)[:10]]
        break

player_info   = {}
dotabuff_info = {}
q1, q2        = queue.Queue(), queue.Queue()
for i, pid in enumerate(ids):
    q1.put((i, pid))
    q2.put(pid)

def api_worker():
    while not q1.empty():
        job = q1.get()
        player_info[job[0]] = get_player_summary(job[1])

def dotabuff_worker():
    while not q2.empty():
        job = q2.get()
        dotabuff_info[job] = get_dotabuff_info(job)

t = []
for i in range(0, 4):
    t.append(threading.Thread(target=api_worker))
    t[i].start()
for i in range(4, 8):
    t.append(threading.Thread(target=dotabuff_worker))
    t[i].start()
for i in range(0, 8):
    t[i].join()

player_names = get_player_name(ids)
for i in range(0, 10):
    if i == 0:
        print("\033[1;32;40mRADIANT\033[0;37;40m")
    if i == 5:
        print("\n\n\n\033[1;31;40mDIRE\033[0;37;40m")

    if not len(player_info[i]):
        print(name_format.format(player_names[ids[i]], dotabuff_info[ids[i]], "PROBABLY A LOSER"))
        print("\t\033[1;31;40mCoward with a private profile\033[0;37;40m")
    else:
        kda = player_info[i]['kills'] + player_info[i]['assists'] / player_info[i]['deaths']
        print(name_format.format(player_names[ids[i]], dotabuff_info[ids[i]], "LOSER" if player_info[i]['wins'] < 5 or kda < 1 else "PROBABLY OK"))
        print("\t{:>12}: {}".format("KDA", kda))
        for nm in non_means:
            print("\t{:>12}: {}".format(title(nm), player_info[i][nm]))
        for m in means:
            print("\t{:>12}: {}".format(title(m), player_info[i][m]))

    if not i == 9:
        print()
