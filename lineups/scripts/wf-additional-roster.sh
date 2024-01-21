#!/bin/bash

YEAR=$1
TEAM=$2

while read n
do
  if grep -q $n tmp/${YEAR}/${TEAM}.links
  then
    i=0
  else
    echo " , ,$n" >> tmp/${YEAR}/${TEAM}.links
  fi
done < tmp/${YEAR}/${TEAM}.app
