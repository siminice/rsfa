#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/t[1-9]*.links
do
  cz-extract-roster.sh $f
  echo $f
done
