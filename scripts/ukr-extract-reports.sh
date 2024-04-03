#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/r/*.html
do
  echo $f
  jrun RuReport $f | tee $f.dat
done
