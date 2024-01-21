#!/bin/bash

YEAR=$1
ANNOTATION=$2

wf-extract-all-reports.sh ${YEAR} ${ANNOTATION} 2> log
cat log | grep -i player | sort | uniq > tmp/${YEAR}/players.log

for f in tmp/${YEAR}/players.log
do
  while read link
  do
    name=$(echo ${link} | cut -d ' ' -f 3)
    curl -s "https://www.worldfootball.net/player_summary/${name}/" > p/${name}.html
    wf-get-player p/${name}.html
  done <  $f >> tmp/${YEAR}/players.extra
done
cat tmp/${YEAR}/players.extra

cat tmp/${YEAR}/players.extra >> tmp/${YEAR}/players.in

wf-add-players.sh ${YEAR}
wf-psort
