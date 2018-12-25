#!/usr/bin/env sh

jo heroes="$(python extr_vpk.py "scripts/npc/npc_heroes.txt" | perl vdf2json.pl | jsonlint -sp | jq -r '.DOTAHeroes | keys[] as $k | try "\(.[$k].HeroID):\(.[$k].url)" catch ""' | sort -n | tail -n+3 | perl fill_missing.pl | sed 's/[0-9]*://g' | jo -a | tr '_' ' ')" items="$(python extr_vpk.py "scripts/npc/items.txt"  | perl vdf2json.pl | jsonlint -sp | jq -r '.DOTAAbilities | keys[] as $k | try "\(.[$k].ID):\(.[$k].ItemAliases)-\($k)" catch ""' | tail -n+2 | perl fix_items.txt.pl | sort -n | perl fill_missing.pl | sed 's/[0-9]*://g' | jo -a)" | jsonlint -sp > data.json
