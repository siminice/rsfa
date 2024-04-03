#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/r/*.html.dat
do
  echo $f
  xm-add-lineups ${YEAR} $f > $f.out
done
