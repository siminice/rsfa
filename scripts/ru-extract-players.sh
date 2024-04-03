#!/bin/bash

YEAR=$1

while read p
do
  if grep -q ",${p}" catalogs/zplayers.dat; then
   ok=1
  else
    jrun BlrPlayer p/${p}.html ${p}
  fi
done < tmp/${YEAR}/unique.pl.links
