#!/bin/bash

LEAGUE=$1
SEASON=$2
N=$3

YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

rm -f tmp/${YEAR}/rep.links

for ((r=1; r<=${N}; r++))
do
  echo "${SEASON} R$r"
  wf-fetch-round.sh ${LEAGUE} ${SEASON} ${r}
done
