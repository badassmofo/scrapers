#!/usr/bin/env bash

if [[ -z $1 ]]; then
  echo 'No ID/Name passed'
  exit 1
fi

if [[ ! -e ~/.steam_api ]]; then
  echo 'No Steam API key!'
  exit 1
fi

if [[ $1 =~ ^[0-9]{17}$ ]]; then
  PARAM="account_id=$1"
else
  PARAM="player_name=$(perl -MURI::Escape -e 'print uri_escape($ARGV[0]);' "$1")"
fi

API=$(wget -qO- "https://api.steampowered.com/IDOTA2Match_570/GetMatchDetails/V001/?key=$(cat ~/.steam_api)&match_id=$(wget -qO- "https://api.steampowered.com/IDOTA2Match_570/GetMatchHistory/V001/?key=$(cat ~/.steam_api)&$PARAM&matches_requested=1" | jq -r '.result.matches[0].match_id')")

JSON=$(echo "{ \"api\": $API, \"data\": $(cat data.json) }")

SCORES=$(echo "$JSON" | jq -r '.api.result.players[] as $p | .data as $d | "\($d.heroes[$p.hero_id]) Level \($p.level), XPM: \($p.xp_per_min), GPM: \($p.gold_per_min), KDA: \($p.kills)/\($p.deaths)/\($p.assists)\n  1. \($d.items[$p.item_0])\n  2. \($d.items[$p.item_1])\n  3. \($d.items[$p.item_2])\n  4. \($d.items[$p.item_3])\n  5. \($d.items[$p.item_4])\n  6. \($d.items[$p.item_5])"' | sed -e 's/null/Nothing/g')

if [[ $(echo "$API" | jq -r '.result.radiant_win') == "false" ]]; then
  echo "$(printf "\e[101m")DIRE VICTORY!$(printf "\e[0m")"
else
  echo "$(printf "\e[102m")RADIANT VICTORY!$(printf "\e[0m")"
fi

echo "\n$(printf "\e[102m")RADIANT:$(printf "\e[0m")\n\n$(printf "$SCORES" | head -n 35)\n\n$(printf "\e[101m")DIRE:$(printf "\e[0m")\n\n$(printf "$SCORES" | tail -n 35)"
