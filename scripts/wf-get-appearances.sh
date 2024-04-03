#!/bin/bash

LEAGUE=$1
SEASON=$2
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi
ANNOTATION=$3

while read t
do
  echo "Appearances for $t"
  if [ ! -f tmp/${YEAR}/${t}.app ]; then
  curl -s "https://www.worldfootball.net/team_performance/${t}/${LEAGUE}-${SEASON}${ANNOTATION}/" | grep player_summary | grep -v ${YEAR} | cut -d '"' -f 4 | cut -d '/' -f 3 > tmp/${YEAR}/${t}.app
#  curl -s "https://www.worldfootball.net/team_performance/${t}/${LEAGUE}-${SEASON}${ANNOTATION}_2/" | grep player_summary | grep -v ${YEAR} | cut -d '"' -f 4 | cut -d '/' -f 3 > tmp/${YEAR}/${t}.app
  fi
done < tmp/${YEAR}/t.links
