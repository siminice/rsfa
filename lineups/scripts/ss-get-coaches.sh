#!/bin/bash

BASEURL='https://us.soccerway.com/coaches/'
YEAR=$1

grep Coach tmp/${YEAR}/r/*.dat | cut -d ' ' -f 3 | sort | uniq > tmp/${YEAR}/coaches.links

while read link
do
  m=$(echo ${link} | cut -d '/' -f 1)
  r=$(echo ${link} | cut -d '/' -f 2)
  if [ ! -f c/${r}.html ]; then
    curl -s -X GET "${BASEURL}/${link}/" > c/${r}.html
  fi
  jrun Player c/${r}.html ${link}
done < tmp/${YEAR}/coaches.links
