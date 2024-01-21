#!/bin/bash

BASEURL='https://www.magyarfutball.hu/en/szemelyek'
YEAR=$1

grep Referee tmp/${YEAR}/r/*.dat | cut -d ' ' -f 2 | grep -v "match" | sort | uniq > tmp/${YEAR}/referees.links

while read link
do
  m=$(echo ${link} | cut -d '/' -f 1)
  r=$(echo ${link} | cut -d '/' -f 2)
  if [ ! -f r/${r}.html ]; then
    curl -s -k -X GET "${BASEURL}/adatlap/${link}" > r/${r}.html
  fi
  jrun HuPlayer r/${r}.html ${link}
done < tmp/${YEAR}/referees.links
