#!/bin/bash

LEAGUE=$(cat data/ln)
SEASON=$1
N=$2

YEAR=${SEASON}

rm -f tmp/${YEAR}/rep.links

for ((r=1; r<=${N}; r++))
do
  echo "${SEASON} R$r"
  wf-get-round-links.sh ${LEAGUE} ${SEASON} ${r} >> tmp/${YEAR}/rep.links
done
