#!/bin/bash

YEAR=$1
URL="https://www.transfermarkt.us"

for f in tmp/${YEAR}/r/*.dat
do
  grep Referee $f
done | cut -d ':' -f 2 | cut -d ' ' -f 2- | sort | uniq | tee tmp/${YEAR}/referees

rm -f tmp/${YEAR}/referees.in

while read line
do
  name=$(echo ${line} | cut -d '/' -f 1)
  pid=$(echo ${line} | cut -d '/' -f 2)
  if [ ! -z ${pid} ]; then
    if [ ! -f "r/${pid}.html" ]; then
      wget -q "${URL}/${name}/profil/schiedsrichter/${pid}" -O "r/${pid}.html"
    fi
    jrun XmReferee "r/${pid}.html" "${name}/${pid}" >> tmp/${YEAR}/referees.in
  fi
done < tmp/${YEAR}/referees

xm-add-catalog catalogs/zreferees.dat tmp/${YEAR}/referees.in
zz-rsort
