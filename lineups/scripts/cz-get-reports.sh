#!/bin/bash

SEASON=$1
NR=$2
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)

for ((i=1; i<=${NR}; i++))
do
  curl -s "https://www.csfotbal.cz/prvni-liga/sezona/${SEASON}/kolo/${i}" > tmp/${YEAR}/round-${i}.html
done
