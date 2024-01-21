#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi
SN=$2

wget "https://www.footballdatabase.eu/en/competition/overall/${SN}-division_1/${SEASON}/-" -O tmp/${YEAR}/season.html

jrun FrSeason tmp/${YEAR}/season.html > tmp/${YEAR}/teams.in

while read t
do
  TZ=$(echo ${t})
  TG=$(grep -e ",${TZ}$" data/zteams.dat -n)
  TLN=$(echo "${TG}" | cut -d ':' -f 1)
  T=$(head -n ${TLN} data/webteams.dat | tail -n 1 | cut -d ',' -f 2)
  echo ${T}
done < tmp/${YEAR}/teams.in | tee tmp/${YEAR}/t.links
