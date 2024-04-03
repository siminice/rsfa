#!/bin/bash

BASEURL='https://us.soccerway.com/referees/'
YEAR=$1

grep Referee tmp/${YEAR}/r/*.dat | cut -d ' ' -f 2 | sort | uniq > tmp/${YEAR}/referees.links

while read link
do
  m=$(echo ${link} | cut -d '/' -f 1)
  r=$(echo ${link} | cut -d '/' -f 2)
  if [ ! -f c/${r}.html ]; then
    curl -s -X GET "${BASEURL}/${link}/" > r/${r}.html
  fi
  jrun Player r/${r}.html ${link}
done < tmp/${YEAR}/referees.links
