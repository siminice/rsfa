#!/bin/bash

YEAR=$1

while read t
do
  ss-get-roster.sh tmp/${YEAR}/${t}.links
done < tmp/${YEAR}/t.links
