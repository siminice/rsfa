#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/r/*.html
do
  echo $f
  jrun BlrReport $f | tee $f.dat
done
