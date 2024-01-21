#!/bin/bash

YEAR=$1

while read t
do
  hu-get-roster.sh tmp/${YEAR}/${t}.links
done < tmp/${YEAR}/t.links
