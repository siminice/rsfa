#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/r/*.html
do
  jrun XmReport $f | tee $f.dat
done
