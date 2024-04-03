#!/bin/bash

YEAR=$1

while read t
do
 echo "---------------------"
 echo $t
 echo "---------------------"
  wf-extract-roster.sh tmp/${YEAR}/${t}.links
done < tmp/${YEAR}/t.links
