#!/bin/bash

while read line
do
  mnem=$(echo ${line} | cut -d ',' -f 3)
  if grep -q ${mnem} catalogs/cplayers.dat
  then
    i=0
  else
    echo "${mnem}"
  fi
done < $1
