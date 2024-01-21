#!/bin/bash

YEAR=$1
URL="https://www.worldfootball.net"

for f in tmp/${YEAR}/r/*.dat
do
  grep Coach $f
done | cut -d ':' -f 2 | cut -d ' ' -f 2- | sort | uniq | tee tmp/${YEAR}/coaches

rm -f tmp/${YEAR}/coaches.in

while read line
do
  pid=${line}
  if [ ! -z ${pid} ]; then
    if [ ! -f "c/${pid}.html" ]; then
      wget -q "${URL}/player_summary/${pid}/" -O "c/${pid}.html"
    fi
    jrun WfPlayer "c/${pid}.html" "${pid}" >> tmp/${YEAR}/coaches.in
  fi
done < tmp/${YEAR}/coaches

wf-add-catalog catalogs/ccoaches.dat tmp/${YEAR}/coaches.in
wf-csort
