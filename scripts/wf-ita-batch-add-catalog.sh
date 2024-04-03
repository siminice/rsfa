#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/t*.in
do
  tid=$(echo $f | cut -d '/' -f 3 | cut -d '.' -f 1 |  cut -c 2-)
  tln=$((${tid}+2))
  t=$(head -n ${tln} data/webteams.dat | tail -n 1 | cut -d ',' -f 1)
  echo "----------------------------------"
  echo "   ${tid}.${t}"
  echo "----------------------------------"
  wf-ita-add-catalog catalogs/cplayers.dat $f
done
