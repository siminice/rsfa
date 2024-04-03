#!/bin/bash

LEAGUE=$(cat ln.dat)
SEASON=$1
N=$2
ANNOTATION=$3

YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

rm -f tmp/${YEAR}/rep.links

for ((r=1; r<=${N}; r++))
do
  echo "${SEASON} R$r"
  if [ ! -f tmp/${YEAR}/r${ROUND}.html ]; then
    wf-get-round-links.sh ${LEAGUE} ${SEASON} ${r} ${ANNOTATION} >> tmp/${YEAR}/rep.links
  fi
done
