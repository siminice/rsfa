#!/bin/bash

LEAGUE=$1
SEASON=$2
ROUND=$3

YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

#if [ ! -f tmp/${YEAR}/r${ROUND}.html ]; then
 echo "Fetching https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}-spieltag/${ROUND} => lineups/tmp/${YEAR}/r${ROUND}.html ..."
 curl -s -X GET "https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}-spieltag/${ROUND}/" > lineups/tmp/${YEAR}/r${ROUND}.html
# curl -s -X GET "https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}-spieltag_2/${ROUND}/" > lineups/tmp/${YEAR}/r${ROUND}.html
#fi
