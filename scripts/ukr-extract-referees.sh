#!/bin/bash

YEAR=$1

while read p
do
  if grep -q ",${p}" catalogs/zreferees.dat; then
    ok=1
  else
    jrun UkrPlayer r/${p}.html ${p}
  fi
done < tmp/${YEAR}/unique.ref.links
