#!/bin/bash

YEAR=$1

while read t
do
  echo "----------------"
  echo $t
  echo "----------------"
  wf-get-roster tmp/${YEAR}/${t}.html | tee tmp/${YEAR}/${t}.links
done < tmp/${YEAR}/t.links
