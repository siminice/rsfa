#!/bin/bash

YEAR=$1
t=$2

rm -f tmp/${YEAR}/players.in

  while read l
  do
    pos=$(echo $l | cut -d ',' -f 1)
    c=$(echo $l | cut -d ',' -f 2)
    link=$(echo $l | cut -d ',' -f 3)
    name=$(echo $l | cut -d ',' -f 4)
    echo "https://www.csfotbal.cz/prvni-liga/hrac/${link}/${c}"
    if [ ! -f p/player-${c}.html ]; then
      curl -s "https://www.csfotbal.cz/prvni-liga/hrac/${link}/${c}" > p/player-${c}.html
    fi
    java -cp "./src/java:./src/java/lib/jsoup-1.12.2.jar" Bio p/player-${c}.html | tee p2
    dob=$(head -n 1 p2 | cut -d ',' -f 1)
    cty=$(head -n 1 p2 | cut -d ',' -f 2)
    echo "${c},${name},${name},${dob},${cty}, ,${cty}" >> tmp/${YEAR}/t${t}.in
  done < tmp/${YEAR}/t${t}.links
