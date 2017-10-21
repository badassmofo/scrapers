#!/usr/bin/env sh
# one line: wget -O - -q "http://thedoujin.com/index.php/api/pages/$ID?json=1" | jq '.[].file_url' | xargs wget -P $ID

for ID in "$@"
do
  JSON=$(wget -O - -q "http://thedoujin.com/index.php/api/pages/$ID?json=1")
  echo $JSON | jq '.[].file_url' | xargs wget -P $ID
  cd $ID || exit
  echo $JSON | jq '.[].file_url' | rev | cut -d '/' -f1 | rev | gawk 'BEGIN{ a=1 }{ printf "mv \"%s %04d.jpg\n", $0, a++ }' | sh
  cd .. || exit
  zip -r "$ID.cbz" $ID
  rm -rf $ID
done
