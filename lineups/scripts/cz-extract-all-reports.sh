#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/r/*.html.dat
do
  echo $f
  cz-extract-report ${YEAR} $f > $f.out
done
