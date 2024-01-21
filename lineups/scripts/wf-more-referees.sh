#!/bin/bash

YEAR=$1
ANNOTATION=$2

wf-extract-all-reports.sh ${YEAR} ${ANNOTATION} 2> log
cat log | grep -i referee | sort | uniq > tmp/${YEAR}/referees.log

for f in tmp/${YEAR}/referees.log
do
  while read link
  do
    name=$(echo ${link} | cut -d ' ' -f 3)
    curl -s "https://www.worldfootball.net/referee_summary/${name}/" > p/ref-${name}.html
    wf-get-referee p/ref-${name}.html
  done <  $f >> tmp/${YEAR}/referees.extra
done
cat tmp/${YEAR}/referees.extra

cat tmp/${YEAR}/referees.extra >> tmp/${YEAR}/referees.in

wf-add-referees.sh ${YEAR}
wf-rsort
