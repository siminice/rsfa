#!/bin/bash

LEAGUE=$1
SEASON=$2
ROUND=$3
ANNOTATION=$4

YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

if [ ! -f tmp/${YEAR}/r${ROUND}.html ]; then
curl -s -X GET "https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}${ANNOTATION}-spieltag/${ROUND}/" > tmp/${YEAR}/r${ROUND}.html
#curl -s -X GET "https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}${ANNOTATION}-spieltag_2/${ROUND}/" > tmp/${YEAR}/r${ROUND}.html
fi

cat tmp/${YEAR}/r${ROUND}.html | grep "/report/" | cut -d '"' -f 2
