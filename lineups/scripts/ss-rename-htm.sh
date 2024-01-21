#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/*.htm
do
  mv $f ${f}l
done
