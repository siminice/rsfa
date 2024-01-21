#!/bin/bash

BASEURL="https://www.footballdatabase.eu"
YEAR=$1

grep Coach tmp/${YEAR}/r/*.dat | cut -d ' ' -f 3 | grep -v "match" | sort | uniq > tmp/${YEAR}/coaches.links

while read link
do
  m=$(echo ${link} | cut -d '/' -f 1)
  r=$(echo ${link} | cut -d '/' -f 2)
  if [ ! -f c/${r}.html ]; then
    wget --quiet "${BASEURL}/en/player/details/${link}" -O c/${r}.html
  fi
  jrun FrPlayer c/${r}.html ${link}
done < tmp/${YEAR}/coaches.links
