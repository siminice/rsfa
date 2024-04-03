#!/bin/bash

YEAR=$1

while read t
do
  echo ${t}
  wf-extract-roster.sh tmp/${YEAR}/${t}.html
done < tmp/${YEAR}/t.links
