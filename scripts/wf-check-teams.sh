#!/bin/bash

YEAR=$1

while read t
do
  echo "${t} ->"
  grep -a ",${t}" data/webteams.dat | cut -d ',' -f 1
done < tmp/${YEAR}/t.links
