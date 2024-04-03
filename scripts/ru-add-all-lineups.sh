#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/r/*.dat
do
  echo $f
  ru-add-lineups ${YEAR} $f
done
