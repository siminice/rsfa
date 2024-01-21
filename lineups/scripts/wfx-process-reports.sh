#!/bin/bash

YEAR=$1
ANNOTATION=$2

for f in tmp/${YEAR}/r/*.html
do
  echo "$f"
  jrun WfReport $f | tee $f.dat
done
