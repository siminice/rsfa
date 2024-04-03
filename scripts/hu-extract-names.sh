#!/bin/bash


FILE=$1
YEAR=$(echo ${FILE} | cut -d '/' -f 2)
TEAM=$(echo ${FILE} | cut -d '/' -f 3 | cut -d '.' -f 1)

TN=$(grep -n ${TEAM} data/webteams.dat | head -n 1 | cut -d ':' -f 1)
TID=$((${TN}-2))

jrun HuRoster ${FILE} > tmp/${YEAR}/${TEAM}.links
#grep "^C," tmp/${YEAR}/${TEAM}.links > tmp/${YEAR}/${TEAM}.coach
#grep -v "^C," tmp/${YEAR}/${TEAM}.links > cppl
#mv -f cppl tmp/${YEAR}/${TEAM}.links

while read link
do
  post=$(echo ${link} | cut -d ',' -f 1)
  num=$(echo ${link} | cut -d ',' -f 2)
  name=$(echo ${link} | cut -d ',' -f 3)
  pid=$(echo ${name} | cut -d '/' -f 1)
  if [ "${pid}" = "adatlap" ]; then
    pid=$(echo ${name} | cut -d '/' -f 2)
    if [ ! -f p/${pid}.html ]; then
      wget --quiet --no-check-certificate "https://www.magyarfutball.hu/en/szemelyek/${name}" -O p/${pid}.html
    fi
  else
    if [ ! -f p/${pid}.html ]; then
      wget --quiet --no-check-certificate "https://www.magyarfutball.hu/en/szemelyek/adatlap/${name}" -O p/${pid}.html
    fi
  fi
  jrun HuPlayer p/${pid}.html ${name}
done <  tmp/${YEAR}/${TEAM}.links | tee tmp/${YEAR}/t${TID}.in
