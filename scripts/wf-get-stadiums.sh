#!/bin/bash

YEAR=$1

grep venue tmp/${YEAR}/r/*.html | cut -d '"' -f 6 | grep venue | sort | uniq | cut -d '/' -f 3 | tee cps

while read s
do
  if grep -q $s catalogs/cstadium.dat
  then
    i=0
  else
    echo " ---> $s not found."
  fi
done < cps

rm -f cps
