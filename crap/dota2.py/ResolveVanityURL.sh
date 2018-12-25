#!/usr/bin/env sh

if test ! -e ~/.steam_api; then
  echo 'No Steam API key!'
  exit 1
fi

if test -z $1; then
  echo 'No vanity name passed!'
  exit 1
fi

printf "%s" "$(wget -qO- "http://api.steampowered.com/ISteamUser/ResolveVanityURL/v0001/?key=$(cat ~/.steam_api)&vanityurl="$(perl -MURI::Escape -e 'print uri_escape($ARGV[0]);' "$*")"" | jq -r '.response.steamid')"
