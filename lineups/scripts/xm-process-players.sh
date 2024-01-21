#!/bin/bash

YEAR=$1
OPT=$2

for f in tmp/${YEAR}/t*.in
do
  team=$(echo ${f} | cut -d '/' -f 3)
  tn=$(echo ${team} | cut -d '.' -f 1)
  tid=$(echo ${tn} | cut -d 't' -f 2)
  tln=$((${tid}+2))
  name=$(head -n ${tln} data/webteams.dat | tail -n 1 | cut -d ',' -f 1)
  echo "==================================================="
  echo "   ${tid} ${name}"
  echo "==================================================="
  xm-add-catalog catalogs/zplayers.dat $f ${OPT}
done

zz-psort
