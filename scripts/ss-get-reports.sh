#!/bin/bash

YEAR=$1
NR=$2

for ((r=1; r<=${NR}; r++))
do
  echo "Round ${r}"
  jrun Round tmp/${YEAR}/r${r}.html > tmp/${YEAR}/r${r}.links
done

for f in tmp/${YEAR}/r*.links
do
  while read link
  do
   echo ${link}
   m=$(echo ${link} | cut -d '/' -f 12)
   if [ ! -f tmp/${YEAR}/r/${m}.html ]; then
     curl -s -X GET "${link}" > tmp/${YEAR}/r/${m}.html
   fi
  done < $f
done
