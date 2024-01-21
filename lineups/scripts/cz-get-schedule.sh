#!/bin/bash

SEASON=$1
NR=$2
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)

for ((i=1; i<=${NR}; i++))
do
  echo "Round ${i}"
  curl -s "https://www.csfotbal.cz/prvni-liga/sezona/${SEASON}/kolo/${i}" > tmp/${YEAR}/round-${i}.html
  java -cp "./src/java:./src/java/lib/jsoup-1.12.2.jar" Round tmp/${YEAR}/round-${i}.html > tmp/${YEAR}/round-${i}.links
  while read l
  do
    echo ${l}
    URL="https://www.csfotbal.cz"${l}
    n=$(echo ${l} | cut -d '/' -f 7)
    if [ ! -f tmp/${YEAR}/r/${n}.html ]; then
      curl -s ${URL} > tmp/${YEAR}/r/${n}.html
    fi
  done < tmp/${YEAR}/round-${i}.links
done
