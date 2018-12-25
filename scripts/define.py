#!/usr/bin/env python3
import requests, json, sys

headers = {'app_id': '050970e8', 'app_key': '0a27555f13778af70fb10d4df91cbb4d'}
url     = "https://od-api.oxforddictionaries.com:443/api/v1/entries/en/%s"

def handle_senses(senses):
    if "definitions" in senses:
        print("\n%s%s\n" % (', '.join([domain for domain in senses["domains"]]) + ": " if "domains" in senses else "", '\n'.join(senses["definitions"])))
    if "crossReferences" in senses:
        for x, cross_references in enumerate(senses["crossReferences"]):
            print("\n%s, %s: %s" % (cross_references["type"], cross_references["text"], senses["crossReferenceMarkers"][x]))
    if "examples" in senses:
        print("  Examples:\n%s" % '\n'.join(["-> " + ex["text"] for ex in senses["examples"]]))
    if "subsenses" in senses:
        for sub in senses["subsenses"]:
            handle_senses(sub)

for a in sys.argv[1:]:
    r = requests.get(url % a, headers=headers)
    j = json.loads(r.text)

    for res in j["results"]:
        print("%s ->" % res["word"])
        for lex_entries in res["lexicalEntries"]:
            a = ','.join([lep["phoneticSpelling"] for lep in lex_entries["pronunciations"]])
            print("  %s, %s (%s)" % (lex_entries["lexicalCategory"], lex_entries["text"], a))
            for lex_entries_entries in lex_entries["entries"]:
                for senses in lex_entries_entries["senses"]:
                    handle_senses(senses)
