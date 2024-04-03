#!/bin/bash


FILE=$1
YEAR=$(echo ${FILE} | cut -d '/' -f 2)
TEAM=$(echo ${FILE} | cut -d '/' -f 3 | cut -d '.' -f 1)

TN=$(grep -a -n ",${TEAM}" data/webteams.dat | head -n 1 | cut -d ':' -f 1)
TID=$((${TN}-2))
echo " .${TEAM} = t${TN}"

wf-get-roster ${FILE} > tmp/${YEAR}/${TEAM}.links
grep "^C," tmp/${YEAR}/${TEAM}.links > tmp/${YEAR}/${TEAM}.coach
grep -v "^C," tmp/${YEAR}/${TEAM}.links > cppl
mv -f cppl tmp/${YEAR}/${TEAM}.links

wf-additional-roster.sh ${YEAR} ${TEAM}

while read link
do
  post=$(echo ${link} | cut -d ',' -f 1)
  num=$(echo ${link} | cut -d ',' -f 2)
  name=$(echo ${link} | cut -d ',' -f 3)
  if [ ! -f p/${name}.html ]; then
    curl -s "https://www.worldfootball.net/player_summary/${name}/" > p/${name}.html
  fi
  wf-get-player p/${name}.html
done <  tmp/${YEAR}/${TEAM}.links | tee tmp/${YEAR}/t${TID}.in
