#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/r/*.dat
do
  grep Venue $f
done | cut -d ' ' -f 2- | sort | uniq -c | sort -rn > tmp/${YEAR}/stadiums

while read line
do
  stad=$(echo ${line} | cut -d ' ' -f 2-)
  if grep -q ",${stad}$" catalogs/zstadium.dat; then
    Ok=1
  else
    echo "${stad} not found."
  fi
done < tmp/${YEAR}/stadiums
