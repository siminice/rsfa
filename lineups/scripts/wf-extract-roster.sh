#!/bin/bash


FILE=$1
YEAR=$(echo ${FILE} | cut -d '/' -f 2)
TEAM=$(echo ${FILE} | cut -d '/' -f 3 | cut -d '.' -f 1)

TN=$(grep -a -n ",${TEAM}" data/webteams.dat | head -n 1 | cut -d ':' -f 1)
TID=$((${TN}-2))

echo " > TID: ${TID}"
while read link
do
  post=$(echo ${link} | cut -d ',' -f 1)
  num=$(echo ${link} | cut -d ',' -f 2)
  name=$(echo ${link} | cut -d ',' -f 3)
  wf-g2 ${name} ${TID} ${post} ${num}
done <  tmp/${YEAR}/${TEAM}.links | tee seasons/${YEAR}/t${TID}.dat
