#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi


#wget --no-check-certificate "https://www.magyarfutball.hu/en/tabellak/16/nb1-${SEASON}" -O tmp/${YEAR}/season.html
jrun HuSeason tmp/${YEAR}/season.html > tmp/${YEAR}/teams.in

while read t
do
  TZ=$(echo ${t})
  TG=$(grep -e ",${TZ}$" data/zteams.dat -n)
  TLN=$(echo "${TG}" | cut -d ':' -f 1)
  T=$(head -n ${TLN} data/webteams.dat | tail -n 1 | cut -d ',' -f 2)
  echo ${T}
done < tmp/${YEAR}/teams.in | tee tmp/${YEAR}/t.links
