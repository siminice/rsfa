#!/bin/bash

LEAGUE=$(cat data/xmln)
CTTY=$(cat data/xmct)
SEASON=$1
YEAR=$((${SEASON}+1))

FILE="tmp/${YEAR}/players.in"

rm -f ${FILE}

while read line
do
  tid=$(echo ${line} | cut -d ',' -f 1)
  team=$(echo ${line} | cut -d ',' -f 2)
  link=$(echo ${line} | cut -d ',' -f 3)
  tn=$(echo ${link} | cut -d '/' -f 2)
  tg=$(grep -e "/${tid}" data/zteams.dat -n | cut -d ':' -f 1)
  tz=$((${tg}-2))
  echo "${tn},===== ${team} =====,${team}, , , , , " >> ${FILE}
  cat tmp/${YEAR}/t${tz}.in >> ${FILE}
done < tmp/${YEAR}/t.links

xm-add-catalog catalogs/zplayers.dat ${FILE}
