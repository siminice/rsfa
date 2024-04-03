#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

URL="https://www.footballdatabase.eu"

for f in tmp/${YEAR}/r[0-9]*.links
do
  while read link
  do
   echo ${link}
   m=$(echo ${link} | cut -d '/' -f 5-)
   ms=$(echo ${m} | sed -e 's|/|-|g')
   if [ ! -f tmp/${YEAR}/r/${ms}.html ]; then
     curl -s -k -X GET "${URL}${link}" > tmp/${YEAR}/r/${ms}.html
   fi
  done < $f
done
