#!/bin/bash


FILE=$1
YEAR=$(echo ${FILE} | cut -d '/' -f 2)
TEAM=$(echo ${FILE} | cut -d '/' -f 3 | cut -d '.' -f 1)

TN=$(grep -n ${TEAM} data/webteams.dat | head -n 1 | cut -d ':' -f 1)
TID=$((${TN}-2))

jrun FrRoster ${FILE} > tmp/${YEAR}/${TEAM}.links

while read link
do
  post=$(echo ${link} | cut -d ',' -f 1)
  num=$(echo ${link} | cut -d ',' -f 2)
  name=$(echo ${link} | cut -d ',' -f 3)
  if [ ! -f p/${name}.html ]; then
      wget --quiet "https://www.footballdatabase.eu/en/player/details/${name}" -O p/${name}.html
  fi
  jrun FrPlayer p/${name}.html ${name}
done <  tmp/${YEAR}/${TEAM}.links | tee tmp/${YEAR}/t${TID}.in
