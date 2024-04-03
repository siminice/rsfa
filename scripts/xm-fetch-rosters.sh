#!/bin/bash

LEAGUE=$(cat data/xmln)
CTTY=$(cat data/xmct)
SEASON=$1
YEAR=$((${SEASON}+1))

URL="https://www.transfermarkt.us"

while read line
do
  tid=$(echo ${line} | cut -d ',' -f 1)
  team=$(echo ${line} | cut -d ',' -f 2)
  link=$(echo ${line} | cut -d ',' -f 3)
  tn=$(echo ${link} | cut -d '/' -f 2)
  tg=$(grep -e "/${tid}" data/zteams.dat -n | cut -d ':' -f 1)
  tz=$((${tg}-2))
  echo "==================================================="
  echo "   ${team}..."
  echo "==================================================="
  if [ ! -f "tmp/${YEAR}/${tn}.html" ]; then
#    wget "${URL}${link}" -O tmp/${YEAR}/${tn}.html
    curl -s "${URL}${link}" -H 'accept-language: en-US' -H 'user-agent: Mozilla/5.0' --compressed > tmp/${YEAR}/${tn}.html
  fi
  jrun XmRoster tmp/${YEAR}/${tn}.html | tee tmp/${YEAR}/${tn}.links
  xm-fetch-players.sh ${YEAR} tmp/${YEAR}/${tn}.links ${tn} ${tid} | tee tmp/${YEAR}/t${tz}.in
done < tmp/${YEAR}/t.links
