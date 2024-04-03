#!/bin/bash


FILE=$1
YEAR=$(echo ${FILE} | cut -d '/' -f 2)
TEAM=$(echo ${FILE} | cut -d '/' -f 3 | cut -d '.' -f 1)

TN=$(grep -n ${TEAM} data/webteams.dat | head -n 1 | cut -d ':' -f 1)
TID=$((${TN}-2))

jrun Roster ${FILE} > tmp/${YEAR}/${TEAM}.links
grep "^C," tmp/${YEAR}/${TEAM}.links > tmp/${YEAR}/${TEAM}.coach
grep -v "^C," tmp/${YEAR}/${TEAM}.links > cppl
mv -f cppl tmp/${YEAR}/${TEAM}.links

while read link
do
  post=$(echo ${link} | cut -d ',' -f 1)
  num=$(echo ${link} | cut -d ',' -f 2)
  name=$(echo ${link} | cut -d ',' -f 3)
  pid=$(echo ${name} | cut -d '/' -f 2)
  if [ ! -f p/${pid}.html ]; then
    curl -s "https://us.soccerway.com/players/${name}/" > p/${pid}.html
  fi
  jrun Player p/${pid}.html ${name}
done <  tmp/${YEAR}/${TEAM}.links | tee tmp/${YEAR}/t${TID}.in
