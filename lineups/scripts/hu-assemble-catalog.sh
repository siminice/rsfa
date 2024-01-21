#!/bin/bash

YEAR=$1

rm -f catalogs/catalog-${YEAR}.dat
for f in seasons/${YEAR}/*.dat
do
  cat $f >> catalogs/catalog-${YEAR}.dat
done
