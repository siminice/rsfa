#!/bin/bash

BASEURL='https://www.magyarfutball.hu/en/szemelyek'
YEAR=$1

grep Coach tmp/${YEAR}/r/*.dat | cut -d ' ' -f 3 | grep -v "match" | sort | uniq > tmp/${YEAR}/coaches.links

while read link
do
  m=$(echo ${link} | cut -d '/' -f 1)
  r=$(echo ${link} | cut -d '/' -f 2)
  if [ ! -f c/${r}.html ]; then
    wget --no-check-certificate --quiet "${BASEURL}/${link}" -O c/${r}.html
  fi
  jrun HuPlayer c/${r}.html ${link}
done < tmp/${YEAR}/coaches.links
