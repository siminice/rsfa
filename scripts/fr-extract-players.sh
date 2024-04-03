#!/bin/bash

YEAR=$1

for f in tmp/${YEAR}/t[0-9]*.in
do
  echo $f
  zz-add-catalog catalogs/zplayers.dat $f
done

zz-psort
