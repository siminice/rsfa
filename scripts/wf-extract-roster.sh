#!/bin/bash
# Updated for WfRoster.java's 4-field output (post,num,id,slug) -- wf-g2
# itself is untouched, it still matches the catalog on the bare slug, same
# as before; only the field index changed (slug moved from field 3 to 4).

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
  slug=$(echo ${link} | cut -d ',' -f 4)
  wf-g2 ${slug} ${TID} ${post} ${num}
done <  tmp/${YEAR}/${TEAM}.links | tee seasons/${YEAR}/t${TID}.dat
