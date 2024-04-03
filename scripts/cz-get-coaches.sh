#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)


rm -f tmp/${YEAR}/coaches.links
while read l
do
  t=$(echo $l | cut -d ',' -f 1)
  link=$(echo $l | cut -d ',' -f 2)
  echo ${link}
  if [ ! -f tmp/${YEAR}/coach-${t}.html ]; then
    curl -s "https://www.csfotbal.cz${link}/treneri" > tmp/${YEAR}/coach-${t}.html
  fi
  java -cp "./src/java:./src/java/lib/jsoup-1.12.2.jar" Coaches tmp/${YEAR}/coach-${t}.html >> tmp/${YEAR}/coaches.links
done < tmp/${YEAR}/teams.dat

rm -f tmp/${YEAR}/coaches.in
while read l
do
  c=$(echo $l | cut -d ',' -f 1)
  link=$(echo $l | cut -d ',' -f 2)
  name=$(echo $l | cut -d ',' -f 3)
  echo "https://www.csfotbal.cz/prvni-liga/trener/${link}/${c}"
  if [ ! -f c/coach-${c}.html ]; then
    curl -s "https://www.csfotbal.cz/prvni-liga/trener/${link}/${c}" > c/coach-${c}.html
  fi
  java -cp "./src/java:./src/java/lib/jsoup-1.12.2.jar" Bio c/coach-${c}.html | tee p2
  dob=$(head -n 1 p2 | cut -d ',' -f 1)
  cty=$(head -n 1 p2 | cut -d ',' -f 2)
  echo "${c},${name},${name},${dob},${cty}, ,${cty}" >> tmp/${YEAR}/coaches.in
done < tmp/${YEAR}/coaches.links
