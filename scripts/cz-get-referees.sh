#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)

curl -s "https://www.csfotbal.cz/prvni-liga/sezona/${SEASON}/rozhodci" > tmp/${YEAR}/rozhodci.html

java -cp "./src/java:./src/java/lib/jsoup-1.12.2.jar" Referees tmp/${YEAR}/rozhodci.html > tmp/${YEAR}/referees.links

rm -f tmp/${YEAR}/referees.in
while read l
do
  c=$(echo $l | cut -d ',' -f 1)
  link=$(echo $l | cut -d ',' -f 2)
  name=$(echo $l | cut -d ',' -f 3)
  echo "https://www.csfotbal.cz/prvni-liga/rozhodci/${link}/${c}"
  if [ ! -f r/referee-${c}.html ]; then
    curl -s "https://www.csfotbal.cz/prvni-liga/rozhodci/${link}/${c}" > r/referee-${c}.html
  fi
  java -cp "./src/java:./src/java/lib/jsoup-1.12.2.jar" Bio r/referee-${c}.html | tee p2
  dob=$(head -n 1 p2 | cut -d ',' -f 1)
  cty=$(head -n 1 p2 | cut -d ',' -f 2)
  echo "${c},${name},${name},${dob},${cty}, ,${cty}" >> tmp/${YEAR}/referees.in
done < tmp/${YEAR}/referees.links
