#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

URL="https://www.magyarfutball.hu"

SSN=$(echo ${SEASON} | sed -e 's/\-/_/g')

wget '--no-check-certificate' "https://www.magyarfutball.hu/en/merkozesek/bajnoki_merkozesek/nb_i/${SSN}" -O tmp/${YEAR}/reports.html

#grep "/merkozes" tmp/${YEAR}/reports.html  | grep -v ${SEASON} | grep -v ${SSN} | grep -v "Matches" | grep -v "Magyarfutball.hu" | grep -v "/merkozesek/bajnoki" | cut -d '"' -f 2 > tmp/${YEAR}/rep.links
grep "/merkozes" tmp/${YEAR}/reports.html | grep -v "Matches" | grep -v "Magyarfutball.hu" | grep -v "/merkozesek/bajnoki" | cut -d '"' -f 2 > tmp/${YEAR}/rep.links
