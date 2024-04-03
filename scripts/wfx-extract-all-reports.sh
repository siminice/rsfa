#!/bin/bash

YEAR=$1
ANNOTATION=$2

for f in tmp/${YEAR}/r/*.html
do
  echo $f
  wfx-add-lineups ${YEAR} $f ${ANNOTATION} > $f.dat
done
