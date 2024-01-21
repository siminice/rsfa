#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

while read t
do
  TZ=$(echo ${t})
  TG=$(grep -e ",${TZ}$" data/zteams.dat -n)
  TLN=$(echo "${TG}" | cut -d ':' -f 1)
  TN=$(head -n ${TLN} data/webteams.dat | tail -n 1 | cut -d ',' -f 2)

  echo " "
  echo "------------------------------"
  wget "https://www.footballdatabase.eu/en/club/roster/${TZ}/${SEASON}" -O tmp/${YEAR}/${TN}.html

  fr-extract-names.sh tmp/${YEAR}/${TN}.html
done < tmp/${YEAR}/teams.in
