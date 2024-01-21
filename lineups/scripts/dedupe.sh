#!/bin/bash

echo "Usage: dedupe.sh CLONE=$1 ORIGINAL=$2 FY=$3 LY=$4."

CLONE=$1
ORIGI=$2
FY=$3
LY=$4

for ((y=${FY}; y<=${LY}; y++))
do
  sed -i "s/${CLONE}/${ORIGI}/g" catalogs/catalog-$y.dat
  sed -i "s/${CLONE}/${ORIGI}/g" catalogs/ncatalog-$y.dat
  sed -i "s/${CLONE}/${ORIGI}/g" catalogs/catalog-$y.dat
  sed -i "s/${CLONE}/${ORIGI}/g" db/lineups-$y.db
  sed -i "s/${CLONE}/${ORIGI}/g" db/events-$y.db
  new-catalog $y -f
done

 grep ${CLONE} catalogs/*catalog*.dat db/*.db
