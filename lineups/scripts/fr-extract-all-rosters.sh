#!/bin/bash

YEAR=$1

while read t
do
  fr-get-roster.sh tmp/${YEAR}/${t}.links
done < tmp/${YEAR}/t.links
