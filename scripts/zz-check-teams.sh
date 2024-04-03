#!/bin/bash

YEAR=$1

cat tmp/${YEAR}/t.links | cut -d ',' -f 3 | cut -d '/' -f 2,5 > tmp/${YEAR}/t.zid

while read t
do
  echo "${t} ->"
  grep ${t} data/zteams.dat | cut -d ',' -f 1
done < tmp/${YEAR}/t.zid

rm -f tmp/${YEAR}/t.zid
