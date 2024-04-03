#!/bin/bash

YEAR=$1

rm -f tmp/${YEAR}/coaches.in
for f in tmp/${YEAR}/*.coach
do
  while read link
  do
    post=$(echo ${link} | cut -d ',' -f 1)
    num=$(echo ${link} | cut -d ',' -f 2)
    name=$(echo ${link} | cut -d ',' -f 3)
    curl -s "https://www.worldfootball.net/player_summary/${name}/" > p/${name}.html
    wf-get-player p/${name}.html
  done <  $f >> tmp/${YEAR}/coaches.in
done
cat tmp/${YEAR}/coaches.in

wf-add-coaches.sh ${YEAR}
wf-csort
