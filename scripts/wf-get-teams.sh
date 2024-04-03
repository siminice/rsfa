#!/bin/bash

LEAGUE=$(cat data/ln)
SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi
ANNOTATION=$2

curl -s "https://www.worldfootball.net/players/${LEAGUE}-${SEASON}${ANNOTATION}/" | grep -e "teams/.*/${YEAR}/2" | cut -d '"' -f 2 | cut -d '/' -f 3 > tmp/${YEAR}/t.links
#curl -s "http://usa.worldfootball.net/players/${LEAGUE}-${SEASON}${ANNOTATION}_2/" | grep -e "teams/.*/${YEAR}/2" | cut -d '"' -f 2 | cut -d '/' -f 3 > tmp/${YEAR}/t.links

while read t
do
  TN=$(grep -a -n ",${t}" data/webteams.dat | head -n 1 | cut -d ':' -f 1)
  TID=$((${TN}-2))

  echo "Team $t = T#${TID}"
  if [ ! -f tmp/${YEAR}/$t.html ]; then
    curl -s -X GET "https://www.worldfootball.net/teams/${t}/${YEAR}/2/" > tmp/${YEAR}/$t.html
  fi
done < tmp/${YEAR}/t.links

wf-get-appearances.sh ${LEAGUE} ${SEASON} ${ANNOTATION}
