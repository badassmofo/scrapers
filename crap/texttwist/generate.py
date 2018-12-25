import json, sys, re, string

allowed = set(string.ascii_lowercase + string.ascii_uppercase)
words = sorted([w for w in list(set([w.strip() for w in sys.stdin])) if set(w) <= allowed and len(w) > 2], key=len)
print("var words_json = '[\"%s\"]';\nvar words_ranges = {" % '","'.join(words))
last_len = 3
start = 0
end = 0
for i, w in enumerate(words):
    try:
        new_len = len(w)
        if new_len != last_len:
            end = i
            print("%d:[%d, %d]," % (last_len, start, end), end='')
            start = end + 1
            last_len = new_len
    except:
        pass
print("%d:[%d, %d]}" % (len(words[-1]), start, len(words)), end='')
