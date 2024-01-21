#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/r/*.dat
do
  echo $f
  hu-add-lineups ${YEAR} $f
done
