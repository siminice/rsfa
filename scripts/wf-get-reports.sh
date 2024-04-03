#!/bin/bash

YEAR=$1
BASEURL="https://www.worldfootball.net"

while read link
do
 echo ${link}
 m=$(echo ${link} | cut -d '/' -f 3)
 if [ ! -f tmp/${YEAR}/r/${m}.html ]; then
   curl -s -X GET "${BASEURL}${link}" > tmp/${YEAR}/r/${m}.html
 fi
done < tmp/${YEAR}/rep.links
