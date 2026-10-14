#!/bin/bash
# Rewritten for the 2026 redesign: the old "venue" link grep against raw
# match-report HTML is dead. The venue's slug is already sitting in the
# "Venue: <slug> <id>" line every processed report prints (WfReport.java),
# so this reads that instead.

YEAR=$1

grep -h "^Venue: " tmp/${YEAR}/r/*.html.dat | sed 's/^Venue: //' | awk '{print $1}' | sort -u | tee cps

while read s
do
  if grep -q $s catalogs/cstadium.dat
  then
    i=0
  else
    echo " ---> $s not found."
  fi
done < cps

rm -f cps
