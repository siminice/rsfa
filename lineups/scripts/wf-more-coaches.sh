#!/bin/bash

YEAR=$1
ANNOTATION=$2

wf-extract-all-reports.sh ${YEAR} ${ANNOTATION} 2> log
cat log | grep -i coach | sort | uniq > tmp/${YEAR}/coaches.log

  while read link
  do
    name=$(echo ${link} | cut -d ' ' -f 3)
#    if [ ! -f p/${name}.html ]; then
      curl -s "https://www.worldfootball.net/player_summary/${name}/" > p/${name}.html
      wf-get-player p/${name}.html
#    fi
  done < tmp/${YEAR}/coaches.log  > tmp/${YEAR}/coaches.extra

cat tmp/${YEAR}/coaches.extra

cat tmp/${YEAR}/coaches.extra >> tmp/${YEAR}/coaches.in

wf-add-coaches.sh ${YEAR}
wf-csort
