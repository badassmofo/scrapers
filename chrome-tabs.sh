#!/bin/sh
curl -sS http://localhost:9222/json | jq --raw-output '.[].url' | ggrep -v '^chrome-extension'
