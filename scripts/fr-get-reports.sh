#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi
SI=$2
ST=$3
NR=$4

SSN=$(echo ${SEASON} | sed -e 's/\-/_/g')

URL="https://www.footballdatabase.eu"

for ((r=1; r<=${NR}; r++))
do
#  echo "R${r}"
  RN=$((${ST} + ${r} - 1))
  if [ ! -f tmp/${YEAR}/r${ROUND}.html ]; then
    wget "${URL}/en/competition/overall/${SI}-division_1/${SSN}/${RN}-journee_${r}" -O tmp/${YEAR}/r${r}.html
    jrun FrRound tmp/${YEAR}/r${r}.html > tmp/${YEAR}/r${r}.links
  fi
done
