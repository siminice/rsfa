#!/bin/bash

YEAR=$1

grep referee_summ tmp/${YEAR}/r/* | cut -d '"' -f 4 | cut -d "/" -f 3 | sort | uniq > tmp/${YEAR}/refs.dat

while read name
do
  if [ ! -f p/ref-${name}.html ]; then
    curl -s "https://www.worldfootball.net/referee_summary/${name}/" > p/ref-${name}.html
  fi
  wf-get-referee p/ref-${name}.html
done < tmp/${YEAR}/refs.dat | tee tmp/${YEAR}/refs.in

wf-add-referees.sh ${YEAR}
wf-rsort
