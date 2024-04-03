#!/bin/bash

YEAR=$1

NM=$(wc tmp/${YEAR}/rep.links | cut -d ' ' -f 3)

echo "Fetching ${NM} reports..."
while read link
do
  mid=$(echo ${link} | cut -d '/' -f 5)
  if [ ! -f tmp/${YEAR}/r/${mid}.html ]; then
    echo "Fetching ${link}..."
    wget -q "${link}" -O tmp/${YEAR}/r/${mid}.html
  else
    echo "tmp/${YEAR}/r/${mid}.html exists"
  fi
done < tmp/${YEAR}/rep.links
