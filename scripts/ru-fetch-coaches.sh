#!/bin/bash

YEAR=$1
#BASEURL="https://footballfacts.ru/players"
BASEURL="https://footballfacts.ru/person"

while read p
do
  if [ ! -f c/${p}.html ]; then
    echo "Fetching ${p}..."
    wget --quiet "${BASEURL}/${p}" -O c/${p}.html
  fi
done < tmp/${YEAR}/unique.co.links
