#!/bin/bash

LEAGUE=$(cat ln.dat)
SEASON=$1
shift
ANNOTATION=$1
shift
ROUNDS=$*

YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

rm -f tmp/${YEAR}/rep.links

for r in ${ROUNDS}
do
  echo "${SEASON} R $r"
  if [ ! -f tmp/${YEAR}/r${r}.html ]; then
     echo "curl -s -X GET https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}-${ANNOTATION}-${r}/0/ > tmp/${YEAR}/r${r}.html"
     curl -s -X GET "https://www.worldfootball.net/schedule/${LEAGUE}-${SEASON}-${ANNOTATION}-${r}/0/" > tmp/${YEAR}/r${r}.html
  fi
done

for r in ${ROUNDS}
do
  cat tmp/${YEAR}/r${r}.html | grep "/report/" | cut -d '"' -f 2  >> tmp/${YEAR}/rep.links
done
