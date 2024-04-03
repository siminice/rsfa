#!/bin/bash

YEAR=$1

while read p
do
  if grep -q ",${p}" catalogs/zcoaches.dat; then
    ok=1
  else
    jrun BlrPlayer c/${p}.html ${p}
  fi
done < tmp/${YEAR}/unique.co.links
