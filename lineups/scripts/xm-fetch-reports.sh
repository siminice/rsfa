#!/bin/bash

YEAR=$1

URL="https://www.transfermarkt.us"

while read link
do
#/olympiacos-piraeus_ao-kavala/index/spielbericht/2606861
  echo "${link}"
  mid=$(echo ${link} | cut -d '/' -f 5)
  if [ ! -f "tmp/${YEAR}/r/${mid}.html" ]; then
#    wget -q "${URL}${link}" -O tmp/${YEAR}/r/${mid}.html
    curl -s "${URL}${link}"  -H 'accept-language: en-US' -H 'user-agent: Mozilla/5.0' --compressed > tmp/${YEAR}/r/${mid}.html
  fi
done < tmp/${YEAR}/rep.links
