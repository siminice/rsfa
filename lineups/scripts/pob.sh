#!/bin/bash

YEAR=${1:-2017}
FIELD=${2:-6}

for f in ${YEAR}/*.dat
do
 t=$(echo $f | cut -d '/' -f 2 | cut -d '.' -f 1)
 check-dob.sh $t 2017 | cut -d ',' -f ${FIELD}
done | sort | uniq -c | sort -rn
