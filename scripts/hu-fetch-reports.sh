#!/bin/bash

SEASON=$1
YEAR=$(echo ${SEASON} | cut -d '-' -f 2)
if [ -z "${YEAR}" ]
then
  YEAR=$(echo ${SEASON} | cut -d '-' -f 1)
fi

URL="https://www.magyarfutball.hu"

NM=$(wc tmp/${YEAR}/rep.links)
echo "Downloading ${NM} links..."

for f in tmp/${YEAR}/rep.links
do
  while read link
  do
   echo ${link}
   m=$(echo ${link} | cut -d '/' -f 4-)
   ms=$(echo ${m} | sed -e 's|/|-|g')
   if [ ! -f tmp/${YEAR}/r/${ms}.html ]; then
     curl -s -k -X GET "${URL}${link}" > tmp/${YEAR}/r/${ms}.html
   fi
  done < $f
done
