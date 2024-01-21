#!/bin/bash

YEAR=$1
URL="https://www.transfermarkt.us"

for f in tmp/${YEAR}/r/*.dat
do
  grep Coach $f
done | cut -d ':' -f 2 | cut -d ' ' -f 2- | sort | uniq | tee tmp/${YEAR}/coaches

rm -f tmp/${YEAR}/coaches.in

while read line
do
  name=$(echo ${line} | cut -d '/' -f 1)
  pid=$(echo ${line} | cut -d '/' -f 2)
  if [ ! -z ${pid} ]; then
    if [ ! -f "c/${pid}.html" ]; then
      wget -q "${URL}/${name}/profil/trainer/${pid}" -O "c/${pid}.html"
    fi
    jrun AutPlayer "c/${pid}.html" "${name}/${pid}" >> tmp/${YEAR}/coaches.in
  fi
done < tmp/${YEAR}/coaches

xm-add-catalog catalogs/zcoaches.dat tmp/${YEAR}/coaches.in
zz-csort
